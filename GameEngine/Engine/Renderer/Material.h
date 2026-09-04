#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Material.h — PBR surface definition + material cache
// ─────────────────────────────────────────────────────────────────────────────
#include "../Math/Math.h"
#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace GE {

// ─────────────────────────────────────────────────────────────────────────────
// Material — the surface description consumed by the lit PBR shader
// ─────────────────────────────────────────────────────────────────────────────
class Material {
public:
    explicit Material(std::string name = "Default");

    // ── Surface parameters ────────────────────────────────────────────────
    Vec3  baseColor        { 1.0f };
    float metallic         = 0.0f;
    float roughness        = 0.6f;
    Vec3  emissiveColor    { 0.0f };
    float emissiveIntensity = 0.0f;

    // ── Map slots (optional) ───────────────────────────────────────────────
    std::shared_ptr<Texture2D> albedoTexture;   // unit 0
    std::shared_ptr<Texture2D> normalTexture;   // unit 1

    // Convenience factory producing a named default material.
    static std::shared_ptr<Material> Create(const std::string& name);

    [[nodiscard]] const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    // Copies surface params (but not map pointers / name) into another material.
    void CopyParamsTo(Material& other) const;

private:
    std::string m_Name;
};

// ─────────────────────────────────────────────────────────────────────────────
// MaterialLibrary — named cache + helpers for shared materials
// ─────────────────────────────────────────────────────────────────────────────
class MaterialLibrary {
public:
    static MaterialLibrary& Get();

    std::shared_ptr<Material> CreateDefault(const std::string& name);
    void Add(const std::string& key, std::shared_ptr<Material> material);
    std::shared_ptr<Material> Get(const std::string& key);
    [[nodiscard]] bool Exists(const std::string& key) const;

private:
    std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
};

} // namespace GE