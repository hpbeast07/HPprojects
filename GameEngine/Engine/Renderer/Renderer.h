#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Renderer.h — forward renderer: sky, directional+point lights, PCF shadow
// mapping, instanced debug lines, and per-frame statistics.
// ─────────────────────────────────────────────────────────────────────────────
#include "../Math/Math.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Framebuffer.h"
#include <memory>
#include <vector>

namespace GE {

constexpr int kMaxPointLights = 8;

// ── Lights ──────────────────────────────────────────────────────────────────
struct DirectionalLight {
    Vec3  direction  { -0.4f, -1.0f, -0.3f };  // travels FROM sun TOWARD scene
    Vec3  color      { 1.0f };
    float intensity  = 2.2f;
    bool  castShadows = true;
};

struct PointLight {
    Vec3  position  { 0.0f, 3.0f, 0.0f };
    Vec3  color     { 1.0f };
    float intensity = 20.0f;
    float radius    = 10.0f;
};

// ── A single submitted draw ─────────────────────────────────────────────────
struct RenderCommand {
    std::shared_ptr<Mesh>     mesh;
    std::shared_ptr<Material> material;
    Mat4                      model      { 1.0f };
    bool                      castShadow = true;
};

// ── Global visual settings ──────────────────────────────────────────────────
struct RendererConfig {
    bool       shadowsEnabled = true;
    uint32_t   shadowMapSize  = 2048;
    float      shadowStrength = 0.8f;
    Vec3       ambientColor   { 0.03f, 0.04f, 0.06f };
    Vec3       fogColor       { 0.55f, 0.60f, 0.70f };
    float      fogDensity     = 0.0f;             // 0 = no fog
    Vec3       skyHorizon     { 0.52f, 0.58f, 0.70f };
    Vec3       skyZenith      { 0.10f, 0.16f, 0.40f };
    Vec3       skyGround      { 0.28f, 0.24f, 0.20f };
};

// ─────────────────────────────────────────────────────────────────────────────
class Renderer {
public:
    void Init   (const RendererConfig& config = RendererConfig{});
    void Shutdown();

    // ── Frame flow:  Begin → (sky? submit...) → Flush → End ─────────────
    void BeginScene(const PerspectiveCamera& camera);
    void Submit    (const RenderCommand& command);
    void Flush     ();          // shadow pass + lit pass
    void EndScene  ();          // clears queued commands + per-frame stats

    // ── Debug / gizmo lines (submitted every frame, rendered by FlushLines) ─
    void AddLine(const Vec3& a, const Vec3& b, const Vec3& color = Vec3(1.0f));
    void AddBox (const Vec3& min, const Vec3& max, const Mat4& model = Mat4(1.0f),
                 const Vec3& color = Vec3(1.0f, 0.9f, 0.2f));
    void FlushLines();

    // ── Lights ─────────────────────────────────────────────────────────────
    void SetDirectionalLight(const DirectionalLight& light) { m_DirLight = light; }
    void SetPointLights(const std::vector<PointLight>& lights) { m_PointLights = lights; }
    const DirectionalLight& GetDirectionalLight() const { return m_DirLight; }

    void SetConfig(const RendererConfig& config) { m_Config = config; }
    const RendererConfig& GetConfig() const { return m_Config; }

    // ── Frame statistics (opaque pass only) ────────────────────────────────
    [[nodiscard]] uint32_t GetDrawCalls()  const { return m_DrawCalls; }
    [[nodiscard]] uint32_t GetTriangles()  const { return m_Triangles; }
    [[nodiscard]] bool     IsInitialized() const { return m_Initialized; }

private:
    void CreateShaders();
    void CreateShadowResources();
    void DestroyShadowResources();
    void ShadowPass();
    void LitPass();

    std::shared_ptr<Shader> m_ShaderLit;
    std::shared_ptr<Shader> m_ShaderSky;
    std::shared_ptr<Shader> m_ShaderShadow;
    std::shared_ptr<Shader> m_ShaderLine;

    RendererConfig         m_Config;
    std::vector<RenderCommand> m_Commands;
    DirectionalLight       m_DirLight;
    std::vector<PointLight> m_PointLights;

    // Shadow mapping
    GLuint m_ShadowFBO  = 0;
    std::shared_ptr<Texture2D> m_ShadowColor;      // colour render target
    std::shared_ptr<Texture2D> m_ShadowDepth;      // sampled depth map
    Mat4 m_LightSpaceMat { 1.0f };

    // Sky pass needs *some* VAO bound in core profile.
    GLuint m_SkyVAO = 0;

    // Debug lines
    GLuint m_LineVAO = 0;
    GLuint m_LineVBO = 0;
    uint32_t m_LineCapacity = 2048;                  // line pairs
    uint32_t m_LineCount    = 0;
    std::vector<float> m_LineData;                   // pos.xyz, color.rgb per vertex

    // Frame state
    const PerspectiveCamera* m_Camera = nullptr;
    uint32_t m_DrawCalls = 0;
    uint32_t m_Triangles = 0;
    bool m_Initialized  = false;
};

// ─────────────────────────────────────────────────────────────────────────────
// RendererHost — owns the global renderer + render API singleton. Apps keep
// one of these and query RendererApp::Get() for the global renderer.
// ─────────────────────────────────────────────────────────────────────────────
class RendererHost {
public:
    static RendererHost& Get();

    void Init(const RendererConfig& config = RendererConfig{});
    void Shutdown();

    Renderer&       GetRenderer()       { return m_Renderer; }

private:
    RendererHost() = default;
    Renderer m_Renderer;
};

} // namespace GE