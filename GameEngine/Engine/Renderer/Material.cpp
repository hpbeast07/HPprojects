#include "Material.h"
#include "../Core/Logger.h"
#include "../Core/Assert.h"

namespace GE {

Material::Material(std::string name) : m_Name(std::move(name)) {}

std::shared_ptr<Material> Material::Create(const std::string& name) {
    return std::make_shared<Material>(name);
}

void Material::CopyParamsTo(Material& other) const {
    other.baseColor          = baseColor;
    other.metallic           = metallic;
    other.roughness          = roughness;
    other.emissiveColor      = emissiveColor;
    other.emissiveIntensity  = emissiveIntensity;
    other.albedoTexture      = albedoTexture;
    other.normalTexture      = normalTexture;
}

// ─── MaterialLibrary ─────────────────────────────────────────────────────────
MaterialLibrary& MaterialLibrary::Get() {
    static MaterialLibrary instance;
    return instance;
}

std::shared_ptr<Material> MaterialLibrary::CreateDefault(const std::string& name) {
    auto material = Material::Create(name);
    m_Materials[name] = material;
    return material;
}

void MaterialLibrary::Add(const std::string& key, std::shared_ptr<Material> material) {
    GE_ASSERT(material, "MaterialLibrary::Add called with null material");
    m_Materials[key] = std::move(material);
}

std::shared_ptr<Material> MaterialLibrary::Get(const std::string& key) {
    if (auto it = m_Materials.find(key); it != m_Materials.end())
        return it->second;
    GE_WARN("Material '{}' not found in library", key);
    return nullptr;
}

bool MaterialLibrary::Exists(const std::string& key) const {
    return m_Materials.contains(key);
}

} // namespace GE