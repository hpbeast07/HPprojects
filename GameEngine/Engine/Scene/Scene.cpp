#include "Scene.h"

#include "../Core/Assert.h"

#include <glm/gtx/euler_angles.hpp>

#include <algorithm>

namespace GE {

Mat4 Transform::GetLocalMatrix() const {
    const Mat4 rotationMatrix = glm::yawPitchRoll(
        Radians(rotation.y), Radians(rotation.x), Radians(rotation.z));
    return Translate(position) * rotationMatrix * Scale(scale);
}

EntityID Scene::CreateEntity(std::string name) {
    if (name.empty()) name = "Entity";

    const EntityID id = m_NextEntity++;
    Entity entity;
    entity.id = id;
    entity.name = std::move(name);
    m_Entities.emplace(id, std::move(entity));
    return id;
}

bool Scene::DestroyEntity(EntityID entity, bool destroyChildren) {
    if (!IsValid(entity)) return false;

    if (destroyChildren) {
        DestroyRecursive(entity);
        return true;
    }

    const EntityID parent = m_Entities.at(entity).parent;
    const auto children = m_Entities.at(entity).children;
    for (const EntityID child : children)
        SetParent(child, parent);
    RemoveFromParent(entity);
    m_Entities.erase(entity);
    return true;
}

void Scene::Clear() {
    m_Entities.clear();
    m_NextEntity = 1;
}

bool Scene::IsValid(EntityID entity) const {
    return entity != InvalidEntity && m_Entities.contains(entity);
}

Entity* Scene::TryGetEntity(EntityID entity) {
    const auto it = m_Entities.find(entity);
    return it == m_Entities.end() ? nullptr : &it->second;
}

const Entity* Scene::TryGetEntity(EntityID entity) const {
    const auto it = m_Entities.find(entity);
    return it == m_Entities.end() ? nullptr : &it->second;
}

Entity& Scene::GetEntity(EntityID entity) {
    GE_ASSERT(IsValid(entity), "Invalid entity requested");
    return m_Entities.at(entity);
}

const Entity& Scene::GetEntity(EntityID entity) const {
    GE_ASSERT(IsValid(entity), "Invalid entity requested");
    return m_Entities.at(entity);
}

bool Scene::SetParent(EntityID child, EntityID parent) {
    if (!IsValid(child)) return false;
    if (parent != InvalidEntity && !IsValid(parent)) return false;
    if (child == parent || (parent != InvalidEntity && IsDescendantOf(parent, child))) return false;

    RemoveFromParent(child);
    auto& childEntity = m_Entities.at(child);
    childEntity.parent = parent;
    if (parent != InvalidEntity)
        m_Entities.at(parent).children.push_back(child);
    return true;
}

Mat4 Scene::GetWorldTransform(EntityID entity) const {
    const Entity& current = GetEntity(entity);
    const Mat4 local = current.transform.GetLocalMatrix();
    return current.parent == InvalidEntity ? local : GetWorldTransform(current.parent) * local;
}

std::vector<EntityID> Scene::GetRootEntities() const {
    std::vector<EntityID> result;
    result.reserve(m_Entities.size());
    for (const auto& [id, entity] : m_Entities) {
        if (entity.parent == InvalidEntity) result.push_back(id);
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<EntityID> Scene::GetEntities() const {
    std::vector<EntityID> result;
    result.reserve(m_Entities.size());
    for (const auto& [id, entity] : m_Entities) result.push_back(id);
    std::sort(result.begin(), result.end());
    return result;
}

void Scene::ForEach(const std::function<void(Entity&)>& visitor) {
    for (auto& [id, entity] : m_Entities) visitor(entity);
}

void Scene::ForEach(const std::function<void(const Entity&)>& visitor) const {
    for (const auto& [id, entity] : m_Entities) visitor(entity);
}

bool Scene::IsDescendantOf(EntityID candidate, EntityID ancestor) const {
    EntityID current = candidate;
    while (current != InvalidEntity) {
        if (current == ancestor) return true;
        const Entity* entity = TryGetEntity(current);
        current = entity ? entity->parent : InvalidEntity;
    }
    return false;
}

void Scene::RemoveFromParent(EntityID entity) {
    auto& child = m_Entities.at(entity);
    if (child.parent == InvalidEntity) return;

    auto& siblings = m_Entities.at(child.parent).children;
    siblings.erase(std::remove(siblings.begin(), siblings.end(), entity), siblings.end());
    child.parent = InvalidEntity;
}

void Scene::DestroyRecursive(EntityID entity) {
    const auto children = m_Entities.at(entity).children;
    for (const EntityID child : children)
        DestroyRecursive(child);
    RemoveFromParent(entity);
    m_Entities.erase(entity);
}

} // namespace GE