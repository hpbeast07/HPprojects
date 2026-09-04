#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Texture.h — OpenGL DSA texture 2D wrapper, procedural helpers, texture cache
// ─────────────────────────────────────────────────────────────────────────────
#include <glad/gl.h>
#include "../Math/Math.h"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace GE {

enum class TextureFilter {
    Nearest,
    Linear,
    LinearMipmap,
};

enum class TextureWrap {
    Repeat,
    Clamp,
    MirroredRepeat,
};

// ─────────────────────────────────────────────────────────────────────────────
// Texture2D — RAII wrapper around a GL_TEXTURE_2D object (GL 4.5 DSA API)
// ─────────────────────────────────────────────────────────────────────────────
class Texture2D {
public:
    explicit Texture2D(uint32_t width, uint32_t height,
                       GLenum internalFormat = GL_RGBA8,
                       GLenum format = GL_RGBA,
                       GLenum dataType = GL_UNSIGNED_BYTE);
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    void SetData(const void* data, uint32_t width, uint32_t height = 0);
    void SetFilter(TextureFilter filter);
    void SetWrap(TextureWrap wrap);
    void GenerateMipmaps();

    void Bind(uint32_t unit = 0) const;
    void Unbind() const;

    [[nodiscard]] GLuint    GetID()            const { return m_ID; }
    [[nodiscard]] uint32_t  GetWidth()         const { return m_Width; }
    [[nodiscard]] uint32_t  GetHeight()        const { return m_Height; }
    [[nodiscard]] GLenum    GetInternalFormat() const { return m_InternalFormat; }
    [[nodiscard]] bool      IsValid()          const { return m_ID != 0; }

    static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height,
                                             const void* data = nullptr,
                                             GLenum internalFormat = GL_RGBA8,
                                             GLenum format = GL_RGBA,
                                             GLenum dataType = GL_UNSIGNED_BYTE);
    static std::shared_ptr<Texture2D> CreateFromFile(const std::string& path, bool flipY = true);

private:
    void InvalidateParameters();

    GLuint          m_ID              = 0;
    uint32_t        m_Width           = 0;
    uint32_t        m_Height          = 0;
    GLenum          m_InternalFormat  = GL_RGBA8;
    GLenum          m_Format          = GL_RGBA;
    GLenum          m_DataType        = GL_UNSIGNED_BYTE;
    TextureFilter   m_Filter          = TextureFilter::LinearMipmap;
    TextureWrap     m_Wrap            = TextureWrap::Clamp;
    bool            m_HasMipmaps      = false;
};

// ─────────────────────────────────────────────────────────────────────────────
// Procedural texture helpers — no asset files required for placeholder art
// ─────────────────────────────────────────────────────────────────────────────
// Two-tone checkerboard (great for floors / debug scenes).
std::shared_ptr<Texture2D> CreateCheckerTexture(uint32_t size,
                                                uint32_t gridCells,
                                                const Vec4& colorA,
                                                const Vec4& colorB);

// Soft blue-noise-ish speckle texture (good AO/ORM stand-in).
std::shared_ptr<Texture2D> CreateSpeckleTexture(uint32_t size,
                                                const Vec4& tint,
                                                uint32_t seed = 1337);

// ─────────────────────────────────────────────────────────────────────────────
// TextureLibrary — named cache so assets are loaded once
// ─────────────────────────────────────────────────────────────────────────────
class TextureLibrary {
public:
    static TextureLibrary& Get();

    std::shared_ptr<Texture2D> LoadFromFile(const std::string& path, bool flipY = true);
    void Add(const std::string& key, std::shared_ptr<Texture2D> texture);
    std::shared_ptr<Texture2D> Get(const std::string& key);
    [[nodiscard]] bool Exists(const std::string& key) const;

private:
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;
};

} // namespace GE