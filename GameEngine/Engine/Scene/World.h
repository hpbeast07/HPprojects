#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// World.h — game/runtime scene layer: renderable components attached to Scene
// entities, plus the GameWorld that submits everything through the Renderer.
// ─────────────────────────────────────────────────────────────────────────────
#include "../Math/Math.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Material.h"
#include "Scene.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace GE {

// ── Component: mesh + material attached to an entity ─────────────────────────
struct MeshRendererComponent {
    EntityID entity = InvalidEntity;
    std::shared_ptr<Mesh>     mesh;
    std::shared_ptr<Material> material;
    bool castShadow = true;
    bool visible    = true;
};

// ── Component: point light attached to an entity (position from transform) ───
struct PointLightComponent {
    EntityID  entity = InvalidEntity;
    PointLight light;
};

// ─────────────────────────────────────────────────────────────────────────────
// GameWorld — owns a Scene, a render camera, and the renderable component
// registries. Apps add entities through the factories, then call Render /
// RenderScene every frame.
// ─────────────────────────────────────────────────────────────────────────────
class GameWorld {
public:
    void SetViewportSize(uint32_t width, uint32_t height) {
        m_Camera.SetViewportSize(width, height);
    }

    // ── Entity factories ─────────────────────────────────────────────────
    EntityID CreateEmpty(const std::string& name);

    EntityID AddMesh(const std::string& name,
                     std::shared_ptr<Mesh> mesh,
                     std::shared_ptr<Material> material,
                     const Vec3& position    = Vec3(0.0f),
                     const Vec3& rotationDeg = Vec3(0.0f),
                     const Vec3& scale       = Vec3(1.0f),
                     bool castShadow = true);

    EntityID AddPointLight(const std::string& name,
                           const Vec3& position,
                           const Vec3& color    = Vec3(1.0f),
                           float intensity      = 20.0f,
                           float radius         = 10.0f);

    bool Remove(EntityID entity);
    void Clear();

    // ── Rendering ─────────────────────────────────────────────────────────
    // Begin a frame against the world camera and submit every visible entity.
    // Does NOT flush: apps may still inject overlay lines before Flush().
    void Submit(Renderer& renderer);

    // Submit + flush geometry (and finish the frame).
    void RenderScene(Renderer& renderer);

    // ── Access ────────────────────────────────────────────────────────────
    Scene& GetScene() { return m_Scene; }
    const Scene& GetScene() const { return m_Scene; }

    PerspectiveCamera&       Camera()       { return m_Camera; }
    const PerspectiveCamera& GetCamera() const { return m_Camera; }

    const std::unordered_map<EntityID, MeshRendererComponent>& GetMeshes() const { return m_Meshes; }
    const std::unordered_map<EntityID, PointLightComponent>&   GetLights() const { return m_Lights; }

    MeshRendererComponent* FindMesh(EntityID entity);
    PointLightComponent*   FindLight(EntityID entity);

    std::vector<EntityID> GetMeshEntityIDs() const;

private:
    void CollectPointLights(std::vector<PointLight>& out) const;

    Scene        m_Scene;
    PerspectiveCamera m_Camera;

    std::unordered_map<EntityID, MeshRendererComponent> m_Meshes;
    std::unordered_map<EntityID, PointLightComponent>   m_Lights;
};

} // namespace GE