#include "Renderer.h"
#include "../Core/Logger.h"
#include "../Core/Assert.h"
#include "../Core/Paths.h"

#include <algorithm>
#include <string>

namespace GE {

// ─── Shader loading helper ──────────────────────────────────────────────────
namespace {

std::shared_ptr<Shader> LoadShader(const std::string& name) {
    const auto vert = ResolveAssetPath(std::filesystem::path("Shaders") / (name + ".vert"));
    const auto frag = ResolveAssetPath(std::filesystem::path("Shaders") / (name + ".frag"));
    return ShaderLibrary::Get().Load(vert, frag, name);
}

} // namespace

// ─── Init / Shutdown ───────────────────────────────────────────────────────
void Renderer::Init(const RendererConfig& config) {
    GE_ASSERT(!m_Initialized, "Renderer::Init called twice");
    m_Config = config;
    CreateShaders();
    CreateShadowResources();

    // Line buffer (dynamic) — pos.xyz + color.rgb per vertex.
    m_LineData.resize(m_LineCapacity * 12);
    glCreateVertexArrays(1, &m_LineVAO);
    glCreateBuffers(1, &m_LineVBO);
    glNamedBufferData(m_LineVBO, static_cast<GLsizeiptr>(m_LineData.size() * sizeof(float)),
                      nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexArrayAttrib(m_LineVAO, 0);
    glEnableVertexArrayAttrib(m_LineVAO, 1);
    glVertexArrayAttribFormat(m_LineVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_LineVAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    glVertexArrayAttribBinding(m_LineVAO, 0, 0);
    glVertexArrayAttribBinding(m_LineVAO, 1, 0);
    glVertexArrayVertexBuffer(m_LineVAO, 0, m_LineVBO, 0, 6 * sizeof(float));

    // Sky pass needs a bound VAO in core profile.
    glCreateVertexArrays(1, &m_SkyVAO);

    m_Initialized = true;
    GE_INFO("Renderer initialized (OpenGL forward PBR)");
}

void Renderer::Shutdown() {
    if (!m_Initialized) return;
    if (m_LineVAO) glDeleteVertexArrays(1, &m_LineVAO);
    if (m_LineVBO) glDeleteBuffers(1, &m_LineVBO);
    m_LineVAO = 0;
    m_LineVBO = 0;
    if (m_SkyVAO) glDeleteVertexArrays(1, &m_SkyVAO);
    m_SkyVAO = 0;
    DestroyShadowResources();
    m_ShaderLit.reset();
    m_ShaderSky.reset();
    m_ShaderShadow.reset();
    m_ShaderLine.reset();
    m_Initialized = false;
}

void Renderer::CreateShaders() {
    m_ShaderLit     = LoadShader("lit");
    m_ShaderSky     = LoadShader("sky");
    m_ShaderShadow  = LoadShader("shadow");
    m_ShaderLine    = LoadShader("line");

    GE_ASSERT(m_ShaderLit,    "Failed to load lit shader");
    GE_ASSERT(m_ShaderSky,    "Failed to load sky shader");
    GE_ASSERT(m_ShaderShadow, "Failed to load shadow shader");
    GE_ASSERT(m_ShaderLine,   "Failed to load line shader");
}

// ─── Shadow map resources ───────────────────────────────────────────────────
void Renderer::CreateShadowResources() {
    const uint32_t size = std::max(256u, m_Config.shadowMapSize);

    m_ShadowColor = std::make_shared<Texture2D>(size, size, GL_RGBA8);
    m_ShadowColor->SetFilter(TextureFilter::Linear);
    m_ShadowColor->SetWrap(TextureWrap::Clamp);

    m_ShadowDepth = std::make_shared<Texture2D>(size, size,
                                                GL_DEPTH_COMPONENT32F, GL_DEPTH, GL_FLOAT);
    m_ShadowDepth->SetFilter(TextureFilter::Linear);
    m_ShadowDepth->SetWrap(TextureWrap::Clamp);

    glCreateFramebuffers(1, &m_ShadowFBO);
    glNamedFramebufferTexture(m_ShadowFBO, GL_COLOR_ATTACHMENT0, m_ShadowColor->GetID(), 0);
    glNamedFramebufferTexture(m_ShadowFBO, GL_DEPTH_ATTACHMENT,  m_ShadowDepth->GetID(), 0);
    const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glNamedFramebufferDrawBuffers(m_ShadowFBO, 1, drawBuffers);

    const GLenum status = glCheckNamedFramebufferStatus(m_ShadowFBO, GL_FRAMEBUFFER);
    GE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE,
        "Shadow framebuffer incomplete: {:#x}", static_cast<uint32_t>(status));
    GE_INFO("Shadow map created: {}x{}", size, size);
}

void Renderer::DestroyShadowResources() {
    if (m_ShadowFBO) glDeleteFramebuffers(1, &m_ShadowFBO);
    m_ShadowFBO = 0;
    m_ShadowColor.reset();
    m_ShadowDepth.reset();
}

// ─── Frame flow ─────────────────────────────────────────────────────────────
void Renderer::BeginScene(const PerspectiveCamera& camera) {
    m_Camera = &camera;
    m_Commands.clear();
    m_DrawCalls = 0;
    m_Triangles = 0;
}

void Renderer::Submit(const RenderCommand& command) {
    GE_ASSERT(m_Camera, "Renderer::Submit called outside BeginScene/EndScene");
    if (command.mesh && command.material)
        m_Commands.push_back(command);
}

void Renderer::EndScene() {
    m_Camera = nullptr;
}
// ─── Flush: shadow pass → sky → opaque pass ─────────────────────────────────
void Renderer::Flush() {
    if (!m_Camera) return;

    if (m_Config.shadowsEnabled && m_DirLight.castShadows)
        ShadowPass();

    LitPass();
}

// ─── Shadow pass ────────────────────────────────────────────────────────────
void Renderer::ShadowPass() {
    const uint32_t size = std::max(256u, m_Config.shadowMapSize);

    GLint prevFBO = 0;
    GLint viewport[4] = { 0, 0, 0, 0 };
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFBO);
    glGetIntegerv(GL_VIEWPORT, viewport);

    glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBO);
    glViewport(0, 0, static_cast<GLsizei>(size), static_cast<GLsizei>(size));
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Build a light-space orthographic frustum centred around the viewer.
    const Vec3 dir    = glm::normalize(m_DirLight.direction);
    Vec3       target = m_Camera->GetPosition();
    target.y = 0.0f;
    constexpr float EXTENT = 25.0f;

    const Vec3 eye = target - dir * 45.0f;
    const Mat4 view = glm::lookAt(eye, target, Vec3(0.0f, 1.0f, 0.0f));
    const Mat4 proj = glm::ortho(-EXTENT, EXTENT, -EXTENT, EXTENT, 1.0f, 100.0f);
    m_LightSpaceMat = proj * view;

    m_ShaderShadow->Bind();
    m_ShaderShadow->SetMat4("u_LightSpaceMat", m_LightSpaceMat);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);        // back faces shrink the shadow slightly (antialiasing)
    glFrontFace(GL_CCW);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.25f, 2.0f);

    for (const RenderCommand& command : m_Commands) {
        if (!command.castShadow || !command.mesh) continue;
        m_ShaderShadow->SetMat4("u_Model", command.model);
        command.mesh->Bind();
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(command.mesh->GetIndexCount()),
                       GL_UNSIGNED_INT, nullptr);
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
    glCullFace(GL_BACK);

    // Restore the app render target + viewport
    glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(prevFBO));
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
// ─── Lit pass ───────────────────────────────────────────────────────────────
void Renderer::LitPass() {
    const PerspectiveCamera& camera = *m_Camera;
    const Mat4 viewProj = camera.GetViewProjectionMatrix();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // 1) Sky dome (background — later opaque geometry overdraws it)
    {
        m_ShaderSky->Bind();
        m_ShaderSky->SetMat4("u_InvViewProj", glm::inverse(viewProj));
        m_ShaderSky->SetVec3("u_SunDirection", glm::normalize(m_DirLight.direction));
        m_ShaderSky->SetVec3("u_HorizonColor", m_Config.skyHorizon);
        m_ShaderSky->SetVec3("u_ZenithColor",  m_Config.skyZenith);
        m_ShaderSky->SetVec3("u_GroundColor",  m_Config.skyGround);

        glBindVertexArray(m_SkyVAO);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        m_DrawCalls++;
    }

    // 2) Opaque PBR pass
    m_ShaderLit->Bind();
    m_ShaderLit->SetMat4("u_ViewProj", viewProj);
    m_ShaderLit->SetVec3("u_ViewPos", camera.GetPosition());
    m_ShaderLit->SetVec3("u_AmbientColor", m_Config.ambientColor);
    m_ShaderLit->SetVec3("u_FogColor", m_Config.fogColor);
    m_ShaderLit->SetFloat("u_FogDensity", m_Config.fogDensity);

    // Directional light + shadow uniforms
    const Vec3 dir = glm::normalize(m_DirLight.direction);
    m_ShaderLit->SetVec3("u_DirLight.direction", dir);
    m_ShaderLit->SetVec3("u_DirLight.color", m_DirLight.color);
    m_ShaderLit->SetFloat("u_DirLight.intensity", m_DirLight.intensity);
    m_ShaderLit->SetInt("u_DirLight.castShadows",
                        (m_Config.shadowsEnabled && m_DirLight.castShadows) ? 1 : 0);
    m_ShaderLit->SetMat4("u_LightSpaceMat", m_LightSpaceMat);
    m_ShaderLit->SetVec2("u_ShadowMapSize",
                         Vec2(static_cast<float>(m_Config.shadowMapSize)));
    m_ShaderLit->SetFloat("u_ShadowStrength", m_Config.shadowStrength);

    if (m_Config.shadowsEnabled && m_DirLight.castShadows && m_ShadowDepth) {
        m_ShadowDepth->Bind(2);
        m_ShaderLit->SetInt("u_ShadowMap", 2);
    } else {
        glBindTextureUnit(2, 0);
    }

    // Point lights
    const int lightCount = std::min(static_cast<int>(m_PointLights.size()), kMaxPointLights);
    m_ShaderLit->SetInt("u_PointLightCount", lightCount);
    for (int i = 0; i < lightCount; i++) {
        const PointLight& light = m_PointLights[i];
        const std::string base = "u_PointLights[" + std::to_string(i) + "].";
        m_ShaderLit->SetVec3 (base + "position",  light.position);
        m_ShaderLit->SetVec3 (base + "color",     light.color);
        m_ShaderLit->SetFloat(base + "intensity", light.intensity);
        m_ShaderLit->SetFloat(base + "radius",    light.radius);
    }

    for (const RenderCommand& command : m_Commands) {
        const Mat4 model     = command.model;
        const Mat3 normalMat = Mat3(glm::transpose(glm::inverse(model)));
        const Material& material = *command.material;

        m_ShaderLit->SetMat4("u_Model", model);
        m_ShaderLit->SetMat3("u_NormalMatrix", normalMat);

        m_ShaderLit->SetVec3 ("u_Material.baseColor",         material.baseColor);
        m_ShaderLit->SetFloat("u_Material.metallic",          material.metallic);
        m_ShaderLit->SetFloat("u_Material.roughness",         material.roughness);
        m_ShaderLit->SetVec3 ("u_Material.emissiveColor",     material.emissiveColor);
        m_ShaderLit->SetFloat("u_Material.emissiveIntensity", material.emissiveIntensity);
        m_ShaderLit->SetInt  ("u_UseAlbedo", material.albedoTexture ? 1 : 0);
        m_ShaderLit->SetInt  ("u_UseNormal", material.normalTexture ? 1 : 0);

        if (material.albedoTexture) material.albedoTexture->Bind(0);
        else glBindTextureUnit(0, 0);
        if (material.normalTexture) material.normalTexture->Bind(1);
        else glBindTextureUnit(1, 0);

        command.mesh->Bind();
        if (command.mesh->IsIndexed()) {
            glDrawElements(GL_TRIANGLES,
                           static_cast<GLsizei>(command.mesh->GetIndexCount()),
                           GL_UNSIGNED_INT, nullptr);
            m_Triangles += command.mesh->GetIndexCount() / 3;
        } else {
            glDrawArrays(GL_TRIANGLES, 0,
                         static_cast<GLsizei>(command.mesh->GetVertexCount()));
            m_Triangles += command.mesh->GetVertexCount() / 3;
        }
        m_DrawCalls++;
    }
}
// ─── Debug line rendering ───────────────────────────────────────────────────
void Renderer::AddLine(const Vec3& a, const Vec3& b, const Vec3& color) {
    if (m_LineCount >= m_LineCapacity) return;   // silently drop overflow

    const size_t base = static_cast<size_t>(m_LineCount) * 12;
    m_LineData[base + 0] = a.x; m_LineData[base + 1] = a.y; m_LineData[base + 2] = a.z;
    m_LineData[base + 3] = color.x; m_LineData[base + 4] = color.y; m_LineData[base + 5] = color.z;
    m_LineData[base + 6] = b.x; m_LineData[base + 7] = b.y; m_LineData[base + 8] = b.z;
    m_LineData[base + 9] = color.x; m_LineData[base + 10] = color.y; m_LineData[base + 11] = color.z;
    m_LineCount++;
}

void Renderer::AddBox(const Vec3& min, const Vec3& max, const Mat4& model, const Vec3& color) {
    const Vec3 corners[8] = {
        Vec3(min.x, min.y, min.z), Vec3(max.x, min.y, min.z),
        Vec3(max.x, min.y, max.z), Vec3(min.x, min.y, max.z),
        Vec3(min.x, max.y, min.z), Vec3(max.x, max.y, min.z),
        Vec3(max.x, max.y, max.z), Vec3(min.x, max.y, max.z),
    };
    Vec3 world[8];
    for (int i = 0; i < 8; i++) {
        const Vec4 c = model * Vec4(corners[i], 1.0f);
        world[i] = Vec3(c);
    }

    static const int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7},
    };
    for (const auto& edge : edges)
        AddLine(world[edge[0]], world[edge[1]], color);
}

void Renderer::FlushLines() {
    if (m_LineCount == 0) return;

    glNamedBufferSubData(m_LineVBO, 0,
                         static_cast<GLsizeiptr>(m_LineCount) * 12 * sizeof(float),
                         m_LineData.data());

    m_ShaderLine->Bind();
    if (m_Camera) {
        m_ShaderLine->SetMat4("u_ViewProj", m_Camera->GetViewProjectionMatrix());
    }

    glBindVertexArray(m_LineVAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_LineCount) * 2);
    glBindVertexArray(0);

    m_LineCount = 0;
}

// ─── RendererHost ───────────────────────────────────────────────────────────
RendererHost& RendererHost::Get() {
    static RendererHost instance;
    return instance;
}

void RendererHost::Init(const RendererConfig& config) {
    m_Renderer.Init(config);
}

void RendererHost::Shutdown() {
    m_Renderer.Shutdown();
}

} // namespace GE