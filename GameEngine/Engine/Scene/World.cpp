#include "World.h"
#include "../Core/Logger.h"

#include <algorithm>

namespace GE {

// ─── Factories ───────────────────────────────────────────────────────────────
EntityID GameWorld::CreateEmpty(const std::string& name) {
    return m_Scene.CreateEntity(name);
}

EntityID GameWorld::AddMesh(const std::string& name,
                            std::shared_ptr<Mesh> mesh,
                            std::shared_ptr<Material> material,
                            const Vec3& position,
                            const Vec3& rotationDeg,
                            const Vec3& scale,
                            bool castShadow)
{
    EntityID id = m_Scene.CreateEntity(name);
    Entity&  e  = m_Scene.GetEntity(id);
    e.transform.position = position;
    e.transform.rotation = rotationDeg;
    e.transform.scale    = scale;

    MeshRendererComponent comp;
    comp.entity    = id;
    comp.mesh      = std::move(mesh);
    comp.material  = std::move(material);
    comp.castShadow = castShadow;
    m_Meshes[id] = std::move(comp);
    return id;
}

EntityID GameWorld::AddPointLight(const std::string& name,
                                  const Vec3& position,
                                  const Vec3& color,
                                  float intensity,
                                  float radius)
{
    EntityID id = m_Scene.CreateEntity(name);
    m_Scene.GetEntity(id).transform.position = position;

    PointLightComponent comp;
    comp.entity    = id;
    comp.light     = PointLight{ position, color, intensity, radius };
    m_Lights[id] = comp;
    return id;
}

// ─── Removal / clear ─────────────────────────────────────────────────────────
bool GameWorld::Remove(EntityID entity) {
    m_Meshes.erase(entity);
    m_Lights.erase(entity);
    return m_Scene.DestroyEntity(entity);
}

void GameWorld::Clear() {
    m_Scene.Clear();
    m_Meshes.clear();
    m_Lights.clear();
}

// ─── Rendering ───────────────────────────────────────────────────────────────
void GameWorld::Submit(Renderer& renderer) {
    renderer.BeginScene(m_Camera);

    std::vector<PointLight> lights;
    CollectPointLights(lights);
    renderer.SetPointLights(lights);

    for (const auto& [id, comp] : m_Meshes) {
        if (!comp.visible || !comp.mesh || !comp.material) continue;
        if (!m_Scene.IsValid(id)) continue;

        RenderCommand command;
        command.mesh       = comp.mesh;
        command.material   = comp.material;
        command.model      = m_Scene.GetWorldTransform(id);
        command.castShadow = comp.castShadow;
        renderer.Submit(command);
    }
}

void GameWorld::RenderScene(Renderer& renderer) {
    Submit(renderer);
    renderer.Flush();
    renderer.EndScene();
}

void GameWorld::CollectPointLights(std::vector<PointLight>& out) const {
    out.clear();
    for (const auto& [id, comp] : m_Lights) {
        PointLight light = comp.light;
        if (const Entity* e = m_Scene.TryGetEntity(id))
            light.position = e->transform.position;
        out.push_back(light);
    }
}

// ─── Access ──────────────────────────────────────────────────────────────────
MeshRendererComponent* GameWorld::FindMesh(EntityID entity) {
    auto it = m_Meshes.find(entity);
    return it != m_Meshes.end() ? &it->second : nullptr;
}

PointLightComponent* GameWorld::FindLight(EntityID entity) {
    auto it = m_Lights.find(entity);
    return it != m_Lights.end() ? &it->second : nullptr;
}

std::vector<EntityID> GameWorld::GetMeshEntityIDs() const {
    std::vector<EntityID> ids;
    ids.reserve(m_Meshes.size());
    for (const auto& [id, comp] : m_Meshes)
        ids.push_back(id);
    return ids;
}

} // namespace GE