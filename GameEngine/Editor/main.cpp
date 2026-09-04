// ═════════════════════════════════════════════════════════════════════════════
//  GameEngineEditor — visual editor bootstrapping the GameEngine library.
//  Viewport · Hierarchy · Inspector · Toolbar · Console · Fly camera.
// ═════════════════════════════════════════════════════════════════════════════
#include "Engine/Core/Application.h"
#include "Engine/Core/Logger.h"
#include "Engine/Input/Input.h"
#include "Engine/Input/KeyCodes.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Scene/World.h"
#include <glad/gl.h>
#include <imgui.h>

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cfloat>
#include <cinttypes>
#include <cstring>

// ── Local helpers ─────────────────────────────────────────────────────────────
namespace GE {

static std::shared_ptr<Material> MakeMaterial(const std::string& name,
                                              const Vec3& baseColor,
                                              float metallic  = 0.0f,
                                              float roughness = 0.6f,
                                              const Vec3& emissive = Vec3(0.0f),
                                              float emissiveIntensity = 0.0f)
{
    auto mat = MaterialLibrary::Get().CreateDefault(name);
    mat->baseColor          = baseColor;
    mat->metallic           = metallic;
    mat->roughness          = roughness;
    mat->emissiveColor      = emissive;
    mat->emissiveIntensity  = emissiveIntensity;
    return mat;
}

// Transform a world-space ray into the mesh's local space, then do a classic
// slab (AABB) intersection test. Returns hit distance along the local ray.
static bool IntersectWorldAABB(const Mat4& model, const AABB& bounds,
                               const Vec3& rayOrigin, const Vec3& rayDir,
                               float& outDist)
{
    const Mat4 invModel = glm::inverse(model);
    const Vec3 localOrigin = Vec3(invModel * Vec4(rayOrigin, 1.0f));
    const Vec3 localDir    = glm::normalize(Vec3(invModel * Vec4(rayDir, 0.0f)));

    const Vec3 invDir(1.0f / (localDir.x != 0.0f ? localDir.x : 1e-12f),
                      1.0f / (localDir.y != 0.0f ? localDir.y : 1e-12f),
                      1.0f / (localDir.z != 0.0f ? localDir.z : 1e-12f));
    const Vec3 lo = (bounds.min - localOrigin) * invDir;
    const Vec3 hi = (bounds.max - localOrigin) * invDir;
    const Vec3 tMin = glm::min(lo, hi);
    const Vec3 tMax = glm::max(lo, hi);

    const float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    const float tFar  = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
    if (tNear <= tFar && tFar >= 0.0f) {
        outDist = tNear > 0.0f ? tNear : 0.0f;
        return true;
    }
    return false;
}

// ═════════════════════════════════════════════════════════════════════════════
//  EditorLayer
// ═════════════════════════════════════════════════════════════════════════════
class EditorLayer final : public Layer {
public:
    EditorLayer() : Layer("EditorLayer") {}

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(int key, int action) override;

private:
    void BuildDefaultScene();

    void RenderViewport();
    void DrawGridOverlay   ();
    void DrawSelectionBox  ();
    void DrawLightGizmos   ();

    void UiMainMenuBar();
    void UiToolbar     ();
    void UiHierarchy   ();
    void UiInspector   ();
    void UiViewport    ();
    void UiConsole     ();
    void UiStats       ();

    void   PickFromMouse(const ImVec2& mousePos);
    void   DuplicateSelected();
    void   DeleteSelected();

    // ── State ────────────────────────────────────────────────────────────
    GameWorld       m_World;
    Renderer        m_Renderer;
    RendererHost*   m_Host       = nullptr;

    std::unique_ptr<FlyCameraController> m_FlyCam;
    std::shared_ptr<Framebuffer> m_ViewportFBO;

    uint32_t m_ViewportWidth  = 1280;
    uint32_t m_ViewportHeight = 720;

    EntityID m_Selected   = InvalidEntity;
    bool     m_ShowGrid   = true;
    bool     m_Wireframe  = false;
    bool     m_ShowLightGizmos = true;
    bool     m_Simulating = false;

    bool  m_ViewportHovered = false;
    bool  m_ViewportFocused = false;
    bool  m_ViewportClick   = false;
    ImVec2 m_ViewportOrigin { 0.0f, 0.0f };
    ImVec2 m_ViewportSize   { 0.0f, 0.0f };

    std::vector<EntityID> m_Animated;   // entities spun in simulate mode
    float m_Time = 0.0f;
};

// ═══ Lifecycle ══════════════════════════════════════════════════════════════
void EditorLayer::OnAttach() {
    // Renderer init must happen while a GL context is current (main window).
    m_Host = &RendererHost::Get();
    RendererConfig cfg;
    cfg.shadowsEnabled = true;
    cfg.shadowMapSize  = 2048;
    cfg.skyHorizon     = Vec3(0.58f, 0.62f, 0.72f);
    cfg.skyZenith      = Vec3(0.12f, 0.18f, 0.42f);
    m_Host->Init(cfg);
    m_Renderer = m_Host->GetRenderer();

    m_Renderer.SetDirectionalLight(
        DirectionalLight{ Vec3(-0.45f, -1.0f, -0.35f), Vec3(1.0f, 0.96f, 0.90f), 2.4f, true });

    // Viewport framebuffer (rendered off-screen, shown inside an ImGui image).
    Framebuffer::Spec fbSpec;
    fbSpec.width  = m_ViewportWidth;
    fbSpec.height = m_ViewportHeight;
    fbSpec.colorInternal = GL_RGBA8;
    fbSpec.enableDepth   = true;
    m_ViewportFBO = std::make_shared<Framebuffer>(fbSpec);

    BuildDefaultScene();

    m_FlyCam = std::make_unique<FlyCameraController>(m_World.Camera());
    m_FlyCam->SetMoveSpeed(8.0f);
    m_World.Camera().SetPosition(Vec3(9.0f, 5.0f, 9.0f));
    m_World.Camera().SetYawPitch(-135.0f, -19.0f);

    GE_INFO("Editor attached: {} entities, viewport {}x{}",
            m_World.GetScene().GetEntities().size(), m_ViewportWidth, m_ViewportHeight);
}

void EditorLayer::OnDetach() {
    m_ViewportFBO.reset();
    if (m_Host) {
        m_Host->Shutdown();
        m_Host = nullptr;
    }
}

// ═══ Default scene so the editor boots to something pretty ═══════════════════
void EditorLayer::BuildDefaultScene() {
    // Fresh rebuild — clear anything from a previous arrangement.
    m_World.Clear();
    m_Animated.clear();
    m_Selected = InvalidEntity;

    MeshLibrary& meshes = MeshLibrary::Get();
    GameWorld& world = m_World;

    // Floor
    auto checker = CreateCheckerTexture(128, 16, Vec4(0.30f, 0.32f, 0.34f, 1.0f),
                                             Vec4(0.16f, 0.17f, 0.19f, 1.0f));
    auto floorMat = MaterialLibrary::Get().CreateDefault("Floor");
    floorMat->albedoTexture = checker;
    floorMat->roughness     = 0.85f;
    world.AddMesh("Floor", meshes.Plane(), floorMat,
                  Vec3(0.0f, -0.5f, 0.0f), Vec3(0.0f), Vec3(14.0f, 1.0f, 14.0f));

    // Pillars (left aisle) + spinning accent
    world.AddMesh("Pillar.L", meshes.Cube(),
                  MakeMaterial("PillarMat", Vec3(0.85f, 0.80f, 0.72f), 0.0f, 0.75f),
                  Vec3(-5.0f, 1.25f, -3.0f), Vec3(0.0f), Vec3(0.8f, 3.0f, 0.8f));
    EntityID spinner = world.AddMesh("Spinner", meshes.Cube(),
                  MakeMaterial("SpinnerMat", Vec3(0.95f, 0.45f, 0.20f), 0.1f, 0.35f),
                  Vec3(-5.0f, 3.9f, -3.0f), Vec3(0.0f), Vec3(0.55f));
    m_Animated.push_back(spinner);

    // Rough boulder
    world.AddMesh("Boulder", meshes.Icosphere(),
                  MakeMaterial("BoulderMat", Vec3(0.55f, 0.42f, 0.30f), 0.0f, 0.95f),
                  Vec3(3.2f, 0.7f, 2.0f), Vec3(0.0f), Vec3(1.4f, 0.9f, 1.1f));

    // Metallic showcase spheres
    const Vec3 sphereColors[] = {
        Vec3(0.70f, 0.12f, 0.10f), Vec3(0.08f, 0.62f, 0.30f),
        Vec3(0.10f, 0.35f, 0.85f), Vec3(0.90f, 0.75f, 0.12f),
    };
    for (int i = 0; i < 4; i++) {
        world.AddMesh("Sphere." + std::to_string(i), meshes.Sphere(),
                      MakeMaterial("ShowGlass_" + std::to_string(i), sphereColors[i], 1.0f, 0.22f),
                      Vec3(-2.4f + i * 1.7f, 0.9f, 3.6f), Vec3(0.0f), Vec3(0.9f));
    }

    // Emissive accent
    world.AddMesh("Lantern", meshes.Sphere(),
                  MakeMaterial("Lantern", Vec3(1.0f, 0.85f, 0.6f), 0.0f, 0.4f,
                               Vec3(1.0f, 0.72f, 0.4f), 2.8f),
                  Vec3(4.8f, 0.8f, -3.8f), Vec3(0.0f), Vec3(0.5f));

    // Point lights
    world.AddPointLight("KeyLight",   Vec3(0.0f, 8.0f, 2.0f),  Vec3(1.0f, 0.92f, 0.82f), 42.0f, 20.0f);
    world.AddPointLight("CoolFill",   Vec3(-6.0f, 5.0f, -5.0f), Vec3(0.30f, 0.55f, 1.0f), 26.0f, 16.0f);
    world.AddPointLight("WarmSpot",   Vec3(4.5f, 2.0f, 4.0f),  Vec3(1.0f, 0.65f, 0.30f), 18.0f, 9.0f);

    // Showcase spheres also animate in simulate mode
    for (EntityID id : world.GetMeshEntityIDs()) {
        Entity& e = world.GetScene().GetEntity(id);
        if (e.name.rfind("Sphere.", 0) == 0)
            m_Animated.push_back(id);
    }

    GE_INFO("Default scene built");
}

// ═══ Per-frame update + render ═══════════════════════════════════════════════
void EditorLayer::OnUpdate(float deltaTime) {
    m_Time += deltaTime;

    if (m_Simulating) {
        const float t = m_Time * 0.6f;
        for (EntityID id : m_Animated) {
            Entity* e = m_World.GetScene().TryGetEntity(id);
            if (!e) continue;
            e->transform.rotation.y += 55.0f * deltaTime;
            if (e->name.rfind("Sphere.", 0) == 0)
                e->transform.position.y = 0.9f + 0.5f * std::sin(t + static_cast<float>(id));
        }
    }

    m_FlyCam->OnUpdate(deltaTime, m_ViewportHovered);
    RenderViewport();
}

void EditorLayer::RenderViewport() {
    if (!m_ViewportFBO) return;
    m_ViewportFBO->ResizeChecked(m_ViewportWidth, m_ViewportHeight);
    m_World.SetViewportSize(m_ViewportWidth, m_ViewportHeight);

    m_ViewportFBO->Bind();
    glViewport(0, 0, static_cast<GLsizei>(m_ViewportWidth),
               static_cast<GLsizei>(m_ViewportHeight));
    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_World.Submit(m_Renderer);
    if (m_ShowGrid)            DrawGridOverlay();
    if (m_Selected)            DrawSelectionBox();
    if (m_ShowLightGizmos)     DrawLightGizmos();

    m_Renderer.Flush();
    m_Renderer.FlushLines();
    m_Renderer.EndScene();

    if (m_Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_ViewportFBO->Unbind();
}

void EditorLayer::DrawGridOverlay() {
    constexpr int kExtent = 12;
    for (int i = -kExtent; i <= kExtent; i++) {
        const bool major = (i % 4 == 0);
        const Vec3 color = major ? Vec3(0.34f) : Vec3(0.18f);
        m_Renderer.AddLine(Vec3(static_cast<float>(i), 0.0f, -static_cast<float>(kExtent)),
                           Vec3(static_cast<float>(i), 0.0f,  static_cast<float>(kExtent)), color);
        m_Renderer.AddLine(Vec3(-static_cast<float>(kExtent), 0.0f, static_cast<float>(i)),
                           Vec3( static_cast<float>(kExtent), 0.0f, static_cast<float>(i)), color);
    }
}

void EditorLayer::DrawSelectionBox() {
    const MeshRendererComponent* comp = m_World.FindMesh(m_Selected);
    if (!comp || !comp->mesh) return;

    const AABB bounds = comp->mesh->GetBounds();
    const Mat4 model = m_World.GetScene().GetWorldTransform(m_Selected) *
                       glm::scale(Mat4(1.0f), Vec3(1.03f));
    m_Renderer.AddBox(bounds.min, bounds.max, model, Vec3(1.0f, 0.85f, 0.25f));
}

void EditorLayer::DrawLightGizmos() {
    const Vec3 kAxisX(1.0f, 0.25f, 0.25f);
    const Vec3 kAxisY(0.25f, 1.0f, 0.25f);
    const Vec3 kAxisZ(0.25f, 0.25f, 1.0f);
    const float len = 0.45f;

    for (const auto& [id, lightComp] : m_World.GetLights()) {
        const Entity* e = m_World.GetScene().TryGetEntity(id);
        if (!e) continue;
        const Vec3& p = e->transform.position;
        const Vec3 tint = glm::clamp(lightComp.light.color, 0.3f, 1.0f);
        m_Renderer.AddLine(p, p + Vec3(len, 0.0f, 0.0f), kAxisX);
        m_Renderer.AddLine(p, p + Vec3(0.0f, len, 0.0f), kAxisY);
        m_Renderer.AddLine(p, p + Vec3(0.0f, 0.0f, len), kAxisZ);

        const float s = 0.07f;
        m_Renderer.AddLine(p - Vec3(s, 0, 0), p + Vec3(s, 0, 0), tint);
        m_Renderer.AddLine(p - Vec3(0, s, 0), p + Vec3(0, s, 0), tint);
    }
}

// ═══ ImGui ══════════════════════════════════════════════════════════════════
static const char* kMeshPrimitiveNames[] = {
    "Cube", "Sphere", "Plane", "Icosphere", "Cylinder", "Cone", "Torus",
};

std::shared_ptr<Mesh> PrimitiveByIndex(int idx) {
    MeshLibrary& L = MeshLibrary::Get();
    switch (idx) {
        case 0: return L.Cube();     case 1: return L.Sphere();
        case 2: return L.Plane();    case 3: return L.Icosphere();
        case 4: return L.Cylinder(); case 5: return L.Cone();
        default: return L.Torus();
    }
}

int PrimitiveIndexFor(const std::shared_ptr<Mesh>& p) {
    MeshLibrary& L = MeshLibrary::Get();
    if (p == L.Cube())      return 0;  if (p == L.Sphere())    return 1;
    if (p == L.Plane())     return 2;  if (p == L.Icosphere()) return 3;
    if (p == L.Cylinder())  return 4;  if (p == L.Cone())      return 5;
    if (p == L.Torus())     return 6;
    return -1;
}

void EditorLayer::OnImGuiRender() {
    UiMainMenuBar();
    UiToolbar();

    // Central dockspace host covering the whole work area.
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                                 ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##DockHost", nullptr, hostFlags);
    ImGui::PopStyleVar(3);
    ImGui::DockSpace(ImGui::GetID("EditorDockSpace"));
    ImGui::End();

    UiViewport();
    UiHierarchy();
    UiInspector();
    UiConsole();
    UiStats();
}

void EditorLayer::UiMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Reset Scene", "Ctrl+N")) BuildDefaultScene();
            if (ImGui::MenuItem("Exit", "Alt+F4"))        Application::Get().Close();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Grid",          "G",  &m_ShowGrid);
            ImGui::MenuItem("Wireframe",     "F1", &m_Wireframe);
            ImGui::MenuItem("Light Gizmos",  "L",  &m_ShowLightGizmos);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Simulation")) {
            if (ImGui::MenuItem("Play", "", m_Simulating))      m_Simulating = true;
            if (ImGui::MenuItem("Stop", "", !m_Simulating))     m_Simulating = false;
            ImGui::Separator();
            ImGui::TextDisabled("Right-drag: orbit  ·  WASD+QE: fly");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::Text("GameEngine Editor v0.2");
            ImGui::Text("Forward PBR · PCF shadows · GL 4.5 DSA");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void EditorLayer::UiToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 4.0f));
    ImGui::Begin("##Toolbar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const char* playLabel = m_Simulating ? "■ Stop" : "▶ Play";
    if (ImGui::Button(playLabel)) m_Simulating = !m_Simulating;
    ImGui::SameLine();
    if (ImGui::Button("Reset Scene")) BuildDefaultScene();
    ImGui::SameLine();

    if (ImGui::Button(m_ShowGrid ? "Grid: On" : "Grid: Off"))      m_ShowGrid = !m_ShowGrid;
    ImGui::SameLine();
    if (ImGui::Button(m_Wireframe ? "Wire: On" : "Wire: Off"))     m_Wireframe = !m_Wireframe;
    ImGui::SameLine();
    if (ImGui::Button(m_ShowLightGizmos ? "Gizmos: On" : "Gizmos: Off"))
        m_ShowLightGizmos = !m_ShowLightGizmos;
    ImGui::SameLine();

    ImGui::Text("FPS %d   |   Draw %u tris %u", Application::Get().GetFPS(),
                m_Renderer.GetDrawCalls(), m_Renderer.GetTriangles());
    ImGui::SameLine();
    ImGui::TextDisabled("| %ux%u", m_ViewportWidth, m_ViewportHeight);
    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorLayer::UiHierarchy() {
    ImGui::Begin("Hierarchy");
    ImGui::TextUnformatted("Entities");
    ImGui::SameLine();
    if (ImGui::SmallButton("+ Add")) ImGui::OpenPopup("AddEntity");
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear")) {
        m_Selected = InvalidEntity;
        m_World.Clear();
    }

    if (ImGui::BeginPopup("AddEntity")) {
        auto spawnMesh = [&](const char* name, std::shared_ptr<Mesh> mesh,
                             const Vec3& color) {
            EntityID id = m_World.AddMesh(name, mesh,
                    MakeMaterial(std::string("New") + name, color, 0.0f, 0.6f),
                    Vec3(2.0f, 1.0f, 2.0f), Vec3(0.0f), Vec3(1.0f));
            m_Selected = id;
        };
        if (ImGui::MenuItem("Empty Entity")) {
            m_Selected = m_World.CreateEmpty("NewEntity");
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cube"))     spawnMesh("NewCube",   MeshLibrary::Get().Cube(),   Vec3(0.82f, 0.30f, 0.25f));
        if (ImGui::MenuItem("Sphere"))   spawnMesh("NewSphere", MeshLibrary::Get().Sphere(), Vec3(0.20f, 0.55f, 0.85f));
        if (ImGui::MenuItem("Plane"))    spawnMesh("NewPlane",  MeshLibrary::Get().Plane(),  Vec3(0.40f, 0.42f, 0.45f));
        if (ImGui::MenuItem("Cylinder")) spawnMesh("NewCylinder", MeshLibrary::Get().Cylinder(), Vec3(0.70f, 0.55f, 0.20f));
        ImGui::Separator();
        if (ImGui::MenuItem("Point Light")) {
            m_Selected = m_World.AddPointLight("NewLight", Vec3(2.0f, 3.0f, 2.0f));
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();

    // Roots first, then children — the Scene graph is flat today but we walk
    // it in a stable order so future nesting slots straight in.
    std::vector<EntityID> ordered = m_World.GetScene().GetEntities();
    std::sort(ordered.begin(), ordered.end(), [&](EntityID a, EntityID b) {
        const Entity& ea = m_World.GetScene().GetEntity(a);
        const Entity& eb = m_World.GetScene().GetEntity(b);
        if ((ea.parent == InvalidEntity) != (eb.parent == InvalidEntity))
            return ea.parent == InvalidEntity;
        return a < b;
    });

    ImGui::BeginChild("EntityList");
    for (EntityID id : ordered) {
        const Entity& e = m_World.GetScene().GetEntity(id);
        const bool isMesh  = m_World.FindMesh(id) != nullptr;
        const bool isLight = m_World.FindLight(id) != nullptr;
        std::string label = isMesh ? "[M] " : (isLight ? "[L] " : "[ ] ");
        label += e.name + "##" + std::to_string(id);

        const bool selected = (m_Selected == id);
        if (ImGui::Selectable(label.c_str(), selected)) {
            m_Selected = selected ? InvalidEntity : id;
        }
        if (ImGui::BeginPopupContextItem(label.c_str())) {
            if (ImGui::MenuItem("Duplicate")) DuplicateSelected();
            if (ImGui::MenuItem("Delete"))    DeleteSelected();
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void EditorLayer::UiInspector() {
    ImGui::Begin("Inspector");

    if (m_Selected == InvalidEntity || !m_World.GetScene().IsValid(m_Selected)) {
        ImGui::TextDisabled("No entity selected");
        ImGui::End();
        return;
    }

    Entity& e = m_World.GetScene().GetEntity(m_Selected);

    // Name + header
    static char nameBuf[128];
    std::strncpy(nameBuf, e.name.c_str(), sizeof(nameBuf) - 1);
    nameBuf[sizeof(nameBuf) - 1] = '\0';
    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
        e.name = nameBuf;
    ImGui::Separator();

    // ── Transform ────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", glm::value_ptr(e.transform.position), 0.05f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(e.transform.rotation), 0.25f);
        ImGui::DragFloat3("Scale",    glm::value_ptr(e.transform.scale),    0.02f, 0.01f, 100.0f);

        // Parenting quick-control: a simple "Parent to..." combo operating on
        // the flat scene (children of an entity keep local-space transforms).
        const std::vector<EntityID> ids = m_World.GetScene().GetEntities();
        if (!ids.empty()) {
            std::string preview = "Root";
            for (EntityID id : ids) {
                if (id == e.parent) {
                    preview = m_World.GetScene().GetEntity(id).name;
                    break;
                }
            }
            if (ImGui::BeginCombo("Parent", preview.c_str())) {
                if (ImGui::Selectable("Root (none)", e.parent == InvalidEntity)) {
                    m_World.GetScene().SetParent(m_Selected, InvalidEntity);
                }
                for (EntityID id : ids) {
                    if (id == m_Selected) continue;
                    const bool sel = (id == e.parent);
                    if (ImGui::Selectable(m_World.GetScene().GetEntity(id).name.c_str(), sel)) {
                        m_World.GetScene().SetParent(m_Selected, id);
                    }
                }
                ImGui::EndCombo();
            }
        }
    }

    // ── Mesh renderer ─────────────────────────────────────────────────────
    if (MeshRendererComponent* meshComp = m_World.FindMesh(m_Selected)) {
        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            int current = std::max(0, PrimitiveIndexFor(meshComp->mesh));
            if (ImGui::Combo("Primitive", &current, kMeshPrimitiveNames,
                             IM_ARRAYSIZE(kMeshPrimitiveNames)))
                meshComp->mesh = PrimitiveByIndex(current);
            ImGui::Checkbox("Visible",    &meshComp->visible);
            ImGui::Checkbox("Cast Shadow",&meshComp->castShadow);

            if (meshComp->material) {
                Material& mat = *meshComp->material;
                ImGui::Separator();
                ImGui::TextUnformatted(mat.GetName().c_str());
                ImGui::ColorEdit3("Base Color", glm::value_ptr(mat.baseColor));
                ImGui::SliderFloat("Metallic",  &mat.metallic,  0.0f, 1.0f);
                ImGui::SliderFloat("Roughness", &mat.roughness, 0.0f, 1.0f);
                ImGui::ColorEdit3("Emissive",   glm::value_ptr(mat.emissiveColor));
                ImGui::SliderFloat("Emissive Intensity", &mat.emissiveIntensity, 0.0f, 8.0f);
            }
        }
    }

    // ── Point light ───────────────────────────────────────────────────────
    if (PointLightComponent* lightComp = m_World.FindLight(m_Selected)) {
        if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::ColorEdit3("Color",     glm::value_ptr(lightComp->light.color));
            ImGui::SliderFloat("Intensity",&lightComp->light.intensity, 0.0f, 200.0f);
            ImGui::SliderFloat("Radius",   &lightComp->light.radius,    1.0f, 60.0f);
        }
    }

    // ── Actions ───────────────────────────────────────────────────────────
    ImGui::Separator();
    if (ImGui::Button("Duplicate"))      DuplicateSelected();
    ImGui::SameLine();
    if (ImGui::Button("Delete"))         DeleteSelected();
    ImGui::SameLine();
    if (ImGui::Button("Focus Camera")) {
        m_World.Camera().LookAt(e.transform.position);
    }

    ImGui::End();
}

void EditorLayer::UiViewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const uint32_t newW = std::max(64u, static_cast<uint32_t>(avail.x));
    const uint32_t newH = std::max(64u, static_cast<uint32_t>(avail.y));
    if (newW != m_ViewportWidth || newH != m_ViewportHeight) {
        m_ViewportWidth  = newW;
        m_ViewportHeight = newH;
    }

    if (m_ViewportFBO) {
        const ImVec2 imagePos = ImGui::GetCursorScreenPos();
        const auto colorTex = m_ViewportFBO->GetColorAttachment();
        const ImTextureID texId = static_cast<ImTextureID>(
            colorTex ? colorTex->GetID() : 0);

        // FBO textures are bottom-up in memory; flip V so the editor shows
        // a conventional top-down image.
        ImGui::Image(texId, ImVec2(static_cast<float>(m_ViewportWidth),
                                   static_cast<float>(m_ViewportHeight)),
                     ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        m_ViewportOrigin = imagePos;
        m_ViewportSize   = ImVec2(static_cast<float>(m_ViewportWidth),
                                  static_cast<float>(m_ViewportHeight));

        // Click (not drag) → pick
        const ImVec2 mouse = ImGui::GetMousePos();
        const bool inside = mouse.x >= m_ViewportOrigin.x &&
                            mouse.y >= m_ViewportOrigin.y &&
                            mouse.x <= m_ViewportOrigin.x + m_ViewportSize.x &&
                            mouse.y <= m_ViewportOrigin.y + m_ViewportSize.y;
        const ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        const bool isDrag = dragDelta.x * dragDelta.x + dragDelta.y * dragDelta.y > 9.0f;
        m_ViewportClick = ImGui::IsWindowHovered() && inside && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !isDrag;
    }

    ImGui::End();
    ImGui::PopStyleVar();

    if (m_ViewportClick) {
        m_ViewportClick = false;
        PickFromMouse(ImGui::GetMousePos());
    }
}

void EditorLayer::UiConsole() {
    ImGui::Begin("Console");

    const std::vector<Logger::LogEntry>& entries = Logger::Get().GetEntries();

    static std::vector<ImVec4> levelColors = {
        ImVec4(0.55f, 0.55f, 0.55f, 1.0f), // Trace
        ImVec4(0.70f, 0.70f, 0.70f, 1.0f), // Debug
        ImVec4(0.90f, 0.90f, 0.90f, 1.0f), // Info
        ImVec4(1.00f, 0.85f, 0.45f, 1.0f), // Warn
        ImVec4(1.00f, 0.45f, 0.40f, 1.0f), // Error
        ImVec4(1.00f, 0.25f, 0.25f, 1.0f), // Critical
    };

    const size_t begin = entries.size() > 300 ? entries.size() - 300 : 0;
    ImGui::BeginChild("LogScroll", ImVec2(0, 0), false,
                      ImGuiWindowFlags_NoSavedSettings);
    for (size_t i = begin; i < entries.size(); i++) {
        const Logger::LogEntry& entry = entries[i];
        const int idx = std::min(static_cast<int>(entry.level), 5);
        ImGui::PushStyleColor(ImGuiCol_Text, levelColors[static_cast<size_t>(idx)]);
        ImGui::TextUnformatted(entry.message.c_str());
        ImGui::PopStyleColor();
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 8.0f)
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();
}

void EditorLayer::UiStats() {
    // Overlay stats on the viewport like a HUD.
    if (!m_ViewportFBO) return;
    ImGui::SetNextWindowPos(ImVec2(m_ViewportOrigin.x + 10.0f, m_ViewportOrigin.y + 10.0f));
    ImGui::SetNextWindowBgAlpha(0.55f);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
    ImGui::Begin("##ViewportStats", nullptr, flags);

    const int fps = Application::Get().GetFPS();
    const float frameMs = Application::Get().GetFrameTimeMs();
    const uint32_t draws = m_Renderer.GetDrawCalls();
    const uint32_t tris  = m_Renderer.GetTriangles();
    const float aspect = m_ViewportHeight > 0
        ? static_cast<float>(m_ViewportWidth) / static_cast<float>(m_ViewportHeight)
        : 16.0f / 9.0f;
    const Vec3& pos    = m_World.Camera().GetPosition();

    ImGui::Text("FPS %d  (%.1f ms)", fps, frameMs);
    ImGui::Text("Draw calls %u   tris %u", draws, tris);
    ImGui::Text("Res %ux%u  aspect %.3f", m_ViewportWidth, m_ViewportHeight, aspect);
    ImGui::Text("Cam  (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
    if (m_Simulating) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.45f, 1.0f, 0.55f, 1.0f), "[ SIMULATING ]");
    }
    ImGui::End();
}

// ═══ Picking & editing actions ═══════════════════════════════════════════════
void EditorLayer::PickFromMouse(const ImVec2& mouse) {
    const float px = mouse.x - m_ViewportOrigin.x;
    const float py = mouse.y - m_ViewportOrigin.y;
    if (px < 0.0f || py < 0.0f || px > m_ViewportSize.x || py > m_ViewportSize.y)
        return;

    const PerspectiveCamera& cam = m_World.Camera();
    const Mat4 invVP = glm::inverse(cam.GetViewProjectionMatrix());
    const float ndcX = (px / m_ViewportSize.x) * 2.0f - 1.0f;
    const float ndcY = 1.0f - (py / m_ViewportSize.y) * 2.0f;

    Vec4 nearP = invVP * Vec4(ndcX, ndcY, -1.0f, 1.0f);
    Vec4 farP  = invVP * Vec4(ndcX, ndcY,  1.0f, 1.0f);
    nearP /= nearP.w;
    farP  /= farP.w;

    const Vec3 origin = Vec3(nearP);
    const Vec3 dir    = glm::normalize(Vec3(farP) - origin);

    EntityID best     = InvalidEntity;
    float    bestDist = FLT_MAX;
    for (const auto& [id, comp] : m_World.GetMeshes()) {
        if (!comp.mesh || !comp.visible) continue;
        const Mat4 model = m_World.GetScene().GetWorldTransform(id);
        float dist = 0.0f;
        if (IntersectWorldAABB(model, comp.mesh->GetBounds(), origin, dir, dist) && dist < bestDist) {
            bestDist = dist;
            best = id;
        }
    }

    // Lights are pickable through a small ray-distance threshold.
    if (best == InvalidEntity) {
        float nearestLight = 0.75f;
        for (const auto& [id, lightComp] : m_World.GetLights()) {
            const Entity* e = m_World.GetScene().TryGetEntity(id);
            if (!e) continue;
            const Vec3 to = e->transform.position - origin;
            const float t = glm::dot(to, dir) > 0.0f ? glm::dot(to, dir) : 0.0f;
            const Vec3 closest = origin + dir * t;
            const float dist = glm::length(closest - e->transform.position);
            if (dist < nearestLight) {
                nearestLight = dist;
                best = id;
            }
        }
    }

    m_Selected = best;
    if (best != InvalidEntity)
        GE_INFO("Selected '{}'", m_World.GetScene().GetEntity(best).name);
}

void EditorLayer::DuplicateSelected() {
    if (m_Selected == InvalidEntity || !m_World.GetScene().IsValid(m_Selected)) return;
    Entity& src = m_World.GetScene().GetEntity(m_Selected);
    const Vec3 offset(0.6f, 0.0f, 0.6f);

    EntityID copy = InvalidEntity;
    if (MeshRendererComponent* c = m_World.FindMesh(m_Selected)) {
        copy = m_World.AddMesh(src.name + " Copy", c->mesh, c->material,
                               src.transform.position + offset,
                               src.transform.rotation, src.transform.scale,
                               c->castShadow);
    } else if (PointLightComponent* l = m_World.FindLight(m_Selected)) {
        copy = m_World.AddPointLight(src.name + " Copy",
                                     src.transform.position + offset,
                                     l->light.color, l->light.intensity, l->light.radius);
    } else {
        copy = m_World.CreateEmpty(src.name + " Copy");
        m_World.GetScene().GetEntity(copy).transform = src.transform;
    }
    m_Selected = copy;
}

void EditorLayer::DeleteSelected() {
    if (m_Selected == InvalidEntity) return;
    m_World.Remove(m_Selected);
    m_Selected = InvalidEntity;
}

// ═══ Keyboard shortcuts ══════════════════════════════════════════════════════
void EditorLayer::OnEvent(int key, int action) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_G:      m_ShowGrid = !m_ShowGrid; break;
        case GLFW_KEY_F1:     m_Wireframe = !m_Wireframe; break;
        case GLFW_KEY_L:      m_ShowLightGizmos = !m_ShowLightGizmos; break;
        case GLFW_KEY_DELETE: if (m_ViewportFocused) DeleteSelected(); break;
        case GLFW_KEY_D:
            if (Input::IsKeyDown(KeyCode::LeftControl) || Input::IsKeyDown(KeyCode::RightControl))
                DuplicateSelected();
            break;
        case GLFW_KEY_ESCAPE:
            Input::SetCursorLocked(false);
            break;
        default: break;
    }
}

// ═══ Application entry ═══════════════════════════════════════════════════════
class GameEngineEditorApp final : public Application {
public:
    GameEngineEditorApp()
        : Application(ApplicationSpec{ "GameEngine Editor", 1760, 990, true, true, "editor.log" })
    {}

protected:
    void OnInit() override {
        PushLayer(std::make_shared<EditorLayer>());
    }
};

} // namespace GE

int main() {
    GE::GameEngineEditorApp app;
    app.Run();
    return 0;
}