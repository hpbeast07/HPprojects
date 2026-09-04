#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Scene.h — compact runtime scene graph with stable entity identifiers
// ─────────────────────────────────────────────────────────────────────────────
#include "../Math/Math.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace GE {

using EntityID = uint32_t;
inline constexpr EntityID InvalidEntity = 0;

struct Transform {
    Vec3 position{ 0.0f };
    Vec3 rotation{ 0.0f };
    Vec3 scale{ 1.0f };

    Mat4 GetLocalMatrix() const;
};

struct Entity {
    EntityID id = InvalidEntity;
    std::string name;
    Transform transform;
    EntityID parent = InvalidEntity;
    std::vector<EntityID> children;
    bool enabled = true;
};

class Scene {
public:
    EntityID CreateEntity(std::string name = "Entity");
    bool DestroyEntity(EntityID entity, bool destroyChildren = true);
    void Clear();

    bool IsValid(EntityID entity) const;
    Entity* TryGetEntity(EntityID entity);
    const Entity* TryGetEntity(EntityID entity) const;
    Entity& GetEntity(EntityID entity);
    const Entity& GetEntity(EntityID entity) const;

    bool SetParent(EntityID child, EntityID parent = InvalidEntity);
    Mat4 GetWorldTransform(EntityID entity) const;

    std::vector<EntityID> GetRootEntities() const;
    std::vector<EntityID> GetEntities() const;
    size_t GetEntityCount() const { return m_Entities.size(); }

    void ForEach(const std::function<void(Entity&)>& visitor);
    void ForEach(const std::function<void(const Entity&)>& visitor) const;

private:
    bool IsDescendantOf(EntityID candidate, EntityID ancestor) const;
    void RemoveFromParent(EntityID entity);
    void DestroyRecursive(EntityID entity);

    EntityID m_NextEntity = 1;
    std::unordered_map<EntityID, Entity> m_Entities;
};

} // namespace GE