#include "Texture.h"
#include "../Core/Logger.h"
#include "../Core/Assert.h"

#include <vector>
#include <algorithm>

// stb_image — single public-domain image loader
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace GE {

// ─── Helper: translate engine param enums to GL enums ───────────────────────
static GLint ToGLFilter(TextureFilter filter) {
    switch (filter) {
        case TextureFilter::Nearest:       return GL_NEAREST;
        case TextureFilter::Linear:        return GL_LINEAR;
        case TextureFilter::LinearMipmap:  return GL_LINEAR_MIPMAP_LINEAR;
    }
    return GL_LINEAR;
}

static GLint ToGLWrap(TextureWrap wrap) {
    switch (wrap) {
        case TextureWrap::Repeat:         return GL_REPEAT;
        case TextureWrap::Clamp:          return GL_CLAMP_TO_EDGE;
        case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
    }
    return GL_CLAMP_TO_EDGE;
}

// ─── Constructor ─────────────────────────────────────────────────────────────
Texture2D::Texture2D(uint32_t width, uint32_t height,
                     GLenum internalFormat, GLenum format, GLenum dataType)
    : m_Width(width), m_Height(height),
      m_InternalFormat(internalFormat), m_Format(format), m_DataType(dataType)
{
    GE_ASSERT(width > 0 && height > 0, "Texture2D: invalid size {}x{}", width, height);
    glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
    glTextureStorage2D(m_ID, 1, m_InternalFormat,
                       static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    InvalidateParameters();
}

Texture2D::~Texture2D() {
    if (m_ID) glDeleteTextures(1, &m_ID);
}

// ─── Data upload ─────────────────────────────────────────────────────────────
void Texture2D::SetData(const void* data, uint32_t width, uint32_t height) {
    if (!m_ID) return;
    GE_ASSERT(data, "Texture2D::SetData called with null data");
    if (width == 0) width  = m_Width;
    if (height == 0) height = m_Height;

    glTextureSubImage2D(m_ID, 0, 0, 0,
                        static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                        m_Format, m_DataType, data);
}

// ─── Sampler parameters ──────────────────────────────────────────────────────
void Texture2D::SetFilter(TextureFilter filter) {
    m_Filter = filter;
    InvalidateParameters();
}

void Texture2D::SetWrap(TextureWrap wrap) {
    m_Wrap = wrap;
    InvalidateParameters();
}

void Texture2D::GenerateMipmaps() {
    if (!m_ID || m_HasMipmaps) return;
    glGenerateTextureMipmap(m_ID);
    m_HasMipmaps = true;
}

void Texture2D::InvalidateParameters() {
    if (!m_ID) return;

    glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, ToGLFilter(m_Filter));
    glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER,
                        (m_Filter == TextureFilter::Nearest) ? GL_NEAREST : GL_LINEAR);
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, ToGLWrap(m_Wrap));
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, ToGLWrap(m_Wrap));
    glTextureParameteri(m_ID, GL_TEXTURE_MAX_LEVEL,
                        (m_Filter == TextureFilter::LinearMipmap) ? 8 : 0);
}

// ─── Binding ─────────────────────────────────────────────────────────────────
void Texture2D::Bind(uint32_t unit) const {
    if (!m_ID) return;
    glBindTextureUnit(unit, m_ID);
}

void Texture2D::Unbind() const {
    glBindTextureUnit(0, 0);
}
// ─── Factories ───────────────────────────────────────────────────────────────
std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height,
                                             const void* data,
                                             GLenum internalFormat,
                                             GLenum format,
                                             GLenum dataType)
{
    auto texture = std::make_shared<Texture2D>(width, height, internalFormat, format, dataType);
    if (data) texture->SetData(data, width, height);
    return texture;
}

std::shared_ptr<Texture2D> Texture2D::CreateFromFile(const std::string& path, bool flipY) {
    stbi_set_flip_vertically_on_load(flipY ? 1 : 0);

    int width = 0, height = 0, channels = 0;
    unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!pixels) {
        GE_ERROR("Failed to load texture '{}'", path);
        return nullptr;
    }

    auto texture = std::make_shared<Texture2D>(static_cast<uint32_t>(width),
                                               static_cast<uint32_t>(height),
                                               GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    texture->SetData(pixels, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    texture->SetFilter(TextureFilter::LinearMipmap);
    texture->SetWrap(TextureWrap::Repeat);
    texture->GenerateMipmaps();

    stbi_image_free(pixels);
    GE_INFO("Loaded texture '{}' ({}x{})", path, width, height);
    return texture;
}

// ─── Procedural textures ─────────────────────────────────────────────────────
std::shared_ptr<Texture2D> CreateCheckerTexture(uint32_t size, uint32_t gridCells,
                                                const Vec4& colorA, const Vec4& colorB)
{
    gridCells = std::max(1u, gridCells);
    const uint32_t cellSize = std::max(1u, size / gridCells);

    std::vector<uint8_t> pixels(static_cast<size_t>(size) * size * 4);
    for (uint32_t y = 0; y < size; y++) {
        for (uint32_t x = 0; x < size; x++) {
            const bool light = (((x / cellSize) + (y / cellSize)) & 1) == 0;
            const Vec4& c = light ? colorA : colorB;
            const size_t idx = (static_cast<size_t>(y) * size + x) * 4;
            pixels[idx + 0] = static_cast<uint8_t>(glm::clamp(c.r, 0.0f, 1.0f) * 255.0f);
            pixels[idx + 1] = static_cast<uint8_t>(glm::clamp(c.g, 0.0f, 1.0f) * 255.0f);
            pixels[idx + 2] = static_cast<uint8_t>(glm::clamp(c.b, 0.0f, 1.0f) * 255.0f);
            pixels[idx + 3] = static_cast<uint8_t>(glm::clamp(c.a, 0.0f, 1.0f) * 255.0f);
        }
    }

    auto texture = Texture2D::Create(size, size, pixels.data());
    texture->SetFilter(TextureFilter::LinearMipmap);
    texture->SetWrap(TextureWrap::Repeat);
    texture->GenerateMipmaps();
    return texture;
}

std::shared_ptr<Texture2D> CreateSpeckleTexture(uint32_t size, const Vec4& tint, uint32_t seed) {
    uint32_t state = seed ? seed : 0xA341316Cu;
    auto nextRand = [&state]() {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    };

    std::vector<uint8_t> pixels(static_cast<size_t>(size) * size * 4);
    for (uint32_t y = 0; y < size; y++) {
        for (uint32_t x = 0; x < size; x++) {
            const float n = (nextRand() % 1000) / 1000.0f;
            const size_t idx = (static_cast<size_t>(y) * size + x) * 4;
            pixels[idx + 0] = static_cast<uint8_t>(tint.r * 255.0f * (0.75f + 0.25f * n));
            pixels[idx + 1] = static_cast<uint8_t>(tint.g * 255.0f * (0.75f + 0.25f * n));
            pixels[idx + 2] = static_cast<uint8_t>(tint.b * 255.0f * (0.75f + 0.25f * n));
            pixels[idx + 3] = 255;
        }
    }

    auto texture = Texture2D::Create(size, size, pixels.data());
    texture->SetFilter(TextureFilter::LinearMipmap);
    texture->SetWrap(TextureWrap::Repeat);
    texture->GenerateMipmaps();
    return texture;
}

// ─── TextureLibrary ──────────────────────────────────────────────────────────
TextureLibrary& TextureLibrary::Get() {
    static TextureLibrary instance;
    return instance;
}

std::shared_ptr<Texture2D> TextureLibrary::LoadFromFile(const std::string& path, bool flipY) {
    if (auto it = m_Textures.find(path); it != m_Textures.end())
        return it->second;

    auto texture = Texture2D::CreateFromFile(path, flipY);
    if (texture) m_Textures[path] = texture;
    return texture;
}

void TextureLibrary::Add(const std::string& key, std::shared_ptr<Texture2D> texture) {
    GE_ASSERT(texture, "TextureLibrary::Add called with null texture");
    m_Textures[key] = std::move(texture);
}

std::shared_ptr<Texture2D> TextureLibrary::Get(const std::string& key) {
    if (auto it = m_Textures.find(key); it != m_Textures.end())
        return it->second;
    GE_WARN("Texture '{}' not found in library", key);
    return nullptr;
}

bool TextureLibrary::Exists(const std::string& key) const {
    return m_Textures.contains(key);
}

} // namespace GE