// ═════════════════════════════════════════════════════════════════════════════
//  GameEngineRuntime — standalone first-person collect-sample game.
//  Walk the arena with WASD, jump with Space, gather every gem before
//  the timer runs out. ESC exits.
// ═════════════════════════════════════════════════════════════════════════════
#include "Engine/Core/Application.h"
#include "Engine/Core/Logger.h"
#include "Engine/Input/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Scene/World.h"
#include <GLFW/glfw3.h>

#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

namespace GE {

class GameLayer final : public Layer {
public:
    GameLayer() : Layer("GameLayer") {}

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDt) override;
    void OnEvent(int key, int action) override;

private:
    struct ObData {
        EntityID id  = InvalidEntity;
        Vec3     half;   // scaled half-extents
    };

    void BuildArena();
    void ResetRun();
    void MoveAndSlide(const Vec3& delta);
    bool OverlapsAABB(const Vec3& center, const Vec3& half) const;

    // ── State ────────────────────────────────────────────────────────────
    GameWorld     m_World;
    Renderer      m_Renderer;
    RendererHost* m_Host     = nullptr;

    EntityID m_Player      = InvalidEntity;
    Vec3     m_PlayerPos   { 0.0f, 0.55f, 0.0f };   // centre of the cube
    Vec3     m_Velocity    { 0.0f };

    bool  m_OnGround = false;
    float m_Yaw      = 90.0f;   // looking down -Z at spawn
    float m_Pitch    = 0.0f;

    std::vector<EntityID> m_Gems;
    std::vector<ObData>   m_Obstacles;
    std::vector<ObData>   m_Walls;

    int   m_Score        = 0;
    int   m_GemCount     = 0;
    float m_TimeLeft     = 45.0f;
    bool  m_Finished     = false;
    bool  m_FirstFrame   = true;
};
// ── File-local material factories ────────────────────────────────────────────
static std::shared_ptr<Material> MakeWallMat() {
    auto m = MaterialLibrary::Get().CreateDefault("Wall");
    m->baseColor = Vec3(0.42f, 0.45f, 0.50f);
    m->roughness = 0.8f;
    return m;
}
static std::shared_ptr<Material> MakeObstacleMat() {
    auto m = MaterialLibrary::Get().CreateDefault("Obstacle");
    m->baseColor = Vec3(0.62f, 0.30f, 0.24f);
    m->roughness = 0.7f;
    return m;
}
static std::shared_ptr<Material> MakeGemMat(int index) {
    static const Vec3 cols[8] = {
        Vec3(1.0f, 0.35f, 0.35f), Vec3(0.40f, 1.0f, 0.55f),
        Vec3(0.45f, 0.65f, 1.0f), Vec3(1.0f, 0.85f, 0.30f),
        Vec3(1.0f, 0.55f, 0.90f), Vec3(0.40f, 0.95f, 1.0f),
        Vec3(0.70f, 0.45f, 1.0f), Vec3(1.0f, 0.70f, 0.35f),
    };
    auto m = MaterialLibrary::Get().CreateDefault("Gem");
    m->baseColor         = cols[index % 8];
    m->metallic          = 0.2f;
    m->roughness         = 0.25f;
    m->emissiveColor     = cols[index % 8] * 0.6f;
    m->emissiveIntensity = 1.6f;
    return m;
}
static std::shared_ptr<Material> MakePlayerMat() {
    auto m = MaterialLibrary::Get().CreateDefault("Player");
    m->baseColor = Vec3(0.18f, 0.75f, 0.55f);
    m->metallic  = 0.1f;
    m->roughness = 0.4f;
    return m;
}
// ═══ Scene construction ══════════════════════════════════════════════════════
void GameLayer::BuildArena() {
    MeshLibrary& meshes = MeshLibrary::Get();

    auto checker = CreateCheckerTexture(128, 16, Vec4(0.32f, 0.34f, 0.36f, 1.0f),
                                             Vec4(0.15f, 0.16f, 0.18f, 1.0f));
    auto floorMat = MaterialLibrary::Get().CreateDefault("Floor");
    floorMat->albedoTexture = checker;
    floorMat->roughness     = 0.9f;
    m_World.AddMesh("Floor", meshes.Plane(), floorMat,
                    Vec3(0.0f, -0.5f, 0.0f), Vec3(0.0f), Vec3(30.0f, 1.0f, 30.0f));

    // Boundary walls (four long cubes) — recorded as static colliders.
    struct Wall { Vec3 pos; Vec3 scale; };
    const Wall walls[] = {
        { Vec3(0.0f,   1.0f,  15.0f), Vec3(30.0f, 2.0f, 1.0f) },
        { Vec3(0.0f,   1.0f, -15.0f), Vec3(30.0f, 2.0f, 1.0f) },
        { Vec3( 15.0f, 1.0f,  0.0f),  Vec3(1.0f, 2.0f, 30.0f) },
        { Vec3(-15.0f, 1.0f,  0.0f),  Vec3(1.0f, 2.0f, 30.0f) },
    };
    auto wallMat = MakeWallMat();
    for (const Wall& w : walls) {
        EntityID id = m_World.AddMesh("Wall", meshes.Cube(), wallMat,
                                      w.pos, Vec3(0.0f), w.scale);
        m_Walls.push_back(ObData{ id, w.scale * 0.5f });
    }

    // Obstacles scattered through the arena
    struct Ob { Vec3 pos; Vec3 scale; };
    const Ob obs[] = {
        { Vec3(-6.0f, 1.0f, -4.0f), Vec3(2.0f, 2.0f, 1.0f) },
        { Vec3( 5.0f, 0.9f,  3.0f), Vec3(1.0f, 1.8f, 2.0f) },
        { Vec3( 2.0f, 0.6f, -7.0f), Vec3(3.0f, 1.2f, 1.0f) },
        { Vec3(-8.0f, 0.8f,  6.0f), Vec3(1.2f, 1.6f, 1.2f) },
        { Vec3( 9.0f, 0.7f, -6.0f), Vec3(1.5f, 1.4f, 2.0f) },
        { Vec3(-2.0f, 0.5f,  8.0f), Vec3(2.5f, 1.0f, 1.2f) },
    };
    auto obMat = MakeObstacleMat();
    for (const Ob& o : obs) {
        EntityID id = m_World.AddMesh("Obstacle", meshes.Cube(), obMat,
                                      o.pos, Vec3(0.0f), o.scale);
        m_Obstacles.push_back(ObData{ id, o.scale * 0.5f });
    }

    // Gems — small emissive spheres to collect
    const Vec3 gemPositions[] = {
        Vec3(-4.0f, 1.2f, -5.0f),  Vec3(4.0f, 1.2f, -4.0f),
        Vec3( 8.0f, 1.2f,  2.0f),  Vec3(-9.0f, 1.2f, 1.0f),
        Vec3(-1.0f, 1.2f, -10.0f), Vec3(1.0f, 1.2f, 8.0f),
        Vec3(-7.0f, 1.2f, -9.0f),  Vec3(6.0f, 1.2f, 9.0f),
    };
    for (int i = 0; i < 8; i++) {
        EntityID id = m_World.AddMesh("Gem." + std::to_string(i), meshes.Sphere(),
                                      MakeGemMat(i), gemPositions[i],
                                      Vec3(0.0f), Vec3(0.45f));
        m_Gems.push_back(id);
    }
    m_GemCount = static_cast<int>(m_Gems.size());

    // Player
    m_Player = m_World.AddMesh("Player", meshes.Cube(), MakePlayerMat(),
                               m_PlayerPos, Vec3(0.0f), Vec3(1.0f));

    // Lights
    m_World.AddPointLight("ArenaLight", Vec3(0.0f, 6.0f, 0.0f),
                          Vec3(1.0f, 0.94f, 0.86f), 55.0f, 30.0f);
    m_World.AddPointLight("Alley", Vec3(-10.0f, 4.0f, 8.0f),
                          Vec3(0.35f, 0.60f, 1.0f), 28.0f, 20.0f);
}
// ═══ Physics ═════════════════════════════════════════════════════════════════
bool GameLayer::OverlapsAABB(const Vec3& center, const Vec3& half) const {
    const auto testBox = [&](const ObData& box) {
        const Entity* e = m_World.GetScene().TryGetEntity(box.id);
        if (!e) return false;
        const Vec3 bc = e->transform.position;
        return std::fabs(center.x - bc.x) < half.x + box.half.x &&
               std::fabs(center.y - bc.y) < half.y + box.half.y &&
               std::fabs(center.z - bc.z) < half.z + box.half.z;
    };

    for (const ObData& box : m_Obstacles)
        if (testBox(box)) return true;
    for (const ObData& box : m_Walls)
        if (testBox(box)) return true;
    return false;
}

void GameLayer::MoveAndSlide(const Vec3& delta) {
    constexpr float kHalf = 0.5f;

    // X pass — a collision kills only horizontal X velocity so we slide.
    Vec3 cand = m_PlayerPos; cand.x += delta.x;
    if (OverlapsAABB(cand, Vec3(kHalf))) m_Velocity.x = 0.0f;
    else m_PlayerPos.x = cand.x;

    // Z pass
    cand = m_PlayerPos; cand.z += delta.z;
    if (OverlapsAABB(cand, Vec3(kHalf))) m_Velocity.z = 0.0f;
    else m_PlayerPos.z = cand.z;

    // Y pass — floor, ceiling and grounded state
    cand = m_PlayerPos; cand.y += delta.y;
    if (cand.y < kHalf) {
        cand.y = kHalf;
        m_Velocity.y = 0.0f;
        m_OnGround = true;
    }
    if (OverlapsAABB(cand, Vec3(kHalf))) {
        m_Velocity.y = 0.0f;
        m_OnGround = true;
    } else {
        m_PlayerPos.y = cand.y;
    }
}

void GameLayer::ResetRun() {
    m_PlayerPos = Vec3(0.0f, 0.55f, 0.0f);
    m_Velocity  = Vec3(0.0f);
    m_Yaw       = 90.0f;
    m_Pitch     = 0.0f;
    m_Score     = 0;
    m_TimeLeft  = 45.0f;
    m_Finished  = false;
    for (EntityID id : m_Gems) {
        if (MeshRendererComponent* c = m_World.FindMesh(id)) c->visible = true;
    }
    m_World.Camera().SetYawPitch(m_Yaw, m_Pitch);
}
// ═══ Lifecycle ═══════════════════════════════════════════════════════════════
void GameLayer::OnAttach() {
    RendererConfig cfg;
    cfg.shadowsEnabled = true;
    cfg.shadowMapSize  = 2048;
    RendererHost::Get().Init(cfg);
    m_Renderer = RendererHost::Get().GetRenderer();

    BuildArena();
    const int w = Application::Get().GetWindow().GetWidth();
    const int h = Application::Get().GetWindow().GetHeight();
    m_World.SetViewportSize(static_cast<uint32_t>(w), static_cast<uint32_t>(h));
    m_World.Camera().SetYawPitch(m_Yaw, m_Pitch);
    Input::SetCursorLocked(true);

    GE_INFO("[Runtime] arena ready — {} gems, {} obstacles, {} walls",
            m_GemCount, m_Obstacles.size(), m_Walls.size());
}

void GameLayer::OnDetach() {
    RendererHost::Get().Shutdown();
}

// ═══ Fixed-step simulation ═══════════════════════════════════════════════════
void GameLayer::OnFixedUpdate(float fixedDt) {
    if (m_Finished) {
        // Keep the player frozen but still renderable.
        Entity& player = m_World.GetScene().GetEntity(m_Player);
        player.transform.position = m_PlayerPos;
        return;
    }

    m_TimeLeft -= fixedDt;
    if (m_TimeLeft <= 0.0f) {
        m_TimeLeft = 0.0f;
        m_Finished = true;
        GE_INFO("[Runtime] time up! {} / {} gems collected", m_Score, m_GemCount);
        return;
    }

    // Wish direction from the camera's yaw (kept horizontal).
    Vec3 fwd = m_World.Camera().GetForward();
    fwd.y = 0.0f;
    if (glm::length2(fwd) < 1e-6f) fwd = Vec3(0.0f, 0.0f, -1.0f);
    fwd = glm::normalize(fwd);
    const Vec3 right = Vec3(-fwd.z, 0.0f, fwd.x);

    const float speed = 8.0f;
    Vec3 wish(0.0f);
    if (Input::IsKeyDown(KeyCode::W)) wish += fwd;
    if (Input::IsKeyDown(KeyCode::S)) wish -= fwd;
    if (Input::IsKeyDown(KeyCode::D)) wish += right;
    if (Input::IsKeyDown(KeyCode::A)) wish -= right;
    if (glm::length2(wish) > 1e-6f) wish = glm::normalize(wish) * speed;

    const float accel = 30.0f;
    const float t = accel * fixedDt;
    m_Velocity.x = wish.x + (m_Velocity.x - wish.x) * std::exp(-t);
    m_Velocity.z = wish.z + (m_Velocity.z - wish.z) * std::exp(-t);

    // Jump / gravity
    const bool grounded = (m_PlayerPos.y <= 0.51f);
    if (grounded && Input::IsKeyDown(KeyCode::Space))
        m_Velocity.y = 9.5f;
    m_Velocity.y -= 25.0f * fixedDt;

    MoveAndSlide(m_Velocity * fixedDt);

    // Collect gems
    const Vec3& p = m_PlayerPos;
    for (EntityID id : m_Gems) {
        MeshRendererComponent* c = m_World.FindMesh(id);
        if (!c || !c->visible) continue;
        const Entity* e = m_World.GetScene().TryGetEntity(id);
        if (!e) continue;
        if (glm::length(e->transform.position - p) < 1.35f) {
            c->visible = false;
            m_Score++;
            GE_INFO("[Runtime] gem collected! {}/{}", m_Score, m_GemCount);
            if (m_Score == m_GemCount) {
                m_Finished = true;
                GE_INFO("[Runtime] ★ ALL GEMS COLLECTED in {:.1f}s ★",
                        45.0f - m_TimeLeft);
            }
        }
    }

    Entity& player = m_World.GetScene().GetEntity(m_Player);
    player.transform.position = m_PlayerPos;
}

// ═══ Frame update ════════════════════════════════════════════════════════════
void GameLayer::OnUpdate(float deltaTime) {
    // Mouse look — only when the cursor is locked (FPS mode).
    if (Input::IsCursorLocked()) {
        const Vec2 delta = Input::GetMouseDelta();
        const float sensitivity = 0.08f;
        m_Yaw   += delta.x * sensitivity;
        m_Pitch += -delta.y * sensitivity;
        m_Pitch  = glm::clamp(m_Pitch, -89.0f, 89.0f);
    }

    // Spin gems gently
    const float ang = deltaTime * 55.0f;
    for (EntityID id : m_Gems) {
        MeshRendererComponent* c = m_World.FindMesh(id);
        if (!c || !c->visible) continue;
        Entity& e = m_World.GetScene().GetEntity(id);
        e.transform.rotation.y += ang;
    }

    // Chase camera
    PerspectiveCamera& cam = m_World.Camera();
    const float eyeHeight = 1.6f;
    cam.SetPosition(Vec3(m_PlayerPos.x, m_PlayerPos.y + eyeHeight, m_PlayerPos.z));
    cam.SetYawPitch(m_Yaw, m_Pitch);

    // Window title HUD
    std::ostringstream title;
    title << "GE Runtime — Gems " << m_Score << "/" << m_GemCount
          << "  Time " << std::max(0, static_cast<int>(std::ceil(m_TimeLeft))) << "s";
    if (m_Finished)
        title << (m_Score == m_GemCount
                      ? "  ★ YOU WIN — press R to restart ★"
                      : "  TIME UP — press R to restart");
    glfwSetWindowTitle(Application::Get().GetWindow().GetNativeWindow(),
                       title.str().c_str());

    m_World.RenderScene(m_Renderer);
}

// ═══ Input ═══════════════════════════════════════════════════════════════════
void GameLayer::OnEvent(int key, int action) {
    if (action != GLFW_PRESS) return;
    switch (key) {
        case GLFW_KEY_ESCAPE: Application::Get().Close(); break;
        case GLFW_KEY_R:      ResetRun(); break;
        default: break;
    }
}

// ═══ Application entry ═══════════════════════════════════════════════════════
class GameEngineRuntimeApp final : public Application {
public:
    GameEngineRuntimeApp()
        : Application(ApplicationSpec{ "GameEngine Runtime", 1280, 720,
                                       true, false, "runtime.log" })
    {}

protected:
    void OnInit() override {
        PushLayer(std::make_shared<GameLayer>());
    }
};

} // namespace GE

int main() {
    GE::GameEngineRuntimeApp app;
    app.Run();
    return 0;
}