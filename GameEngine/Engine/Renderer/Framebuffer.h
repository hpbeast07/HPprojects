#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Framebuffer.h — render-to-texture (editor viewport, shadow maps, post-fx)
// ─────────────────────────────────────────────────────────────────────────────
#include "Texture.h"
#include <memory>
#include <vector>

namespace GE {

class Framebuffer {
public:
    struct Spec {
        uint32_t width         = 1600;
        uint32_t height        = 900;
        GLenum   colorInternal = GL_RGBA8;          // color attachment format
        bool     enableDepth   = true;
        bool     enableStencil = false;
    };

    explicit Framebuffer(const Spec& spec);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    void Bind()   const;
    void Unbind() const;

    // Rebuild all attachments at the requested resolution.
    void Resize(uint32_t width, uint32_t height);
    // No-op when the size already matches — call this from a hot viewport.
    void ResizeChecked(uint32_t width, uint32_t height);

    std::shared_ptr<Texture2D> GetColorAttachment(size_t index = 0) const;
    [[nodiscard]] GLuint    GetID()     const { return m_ID; }
    [[nodiscard]] uint32_t  GetWidth()  const { return m_Width; }
    [[nodiscard]] uint32_t  GetHeight() const { return m_Height; }

private:
    void Invalidate();

    Spec     m_Spec;
    GLuint   m_ID = 0;
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;

    std::vector<std::shared_ptr<Texture2D>> m_ColorAttachments;
    GLuint m_DepthStencilRBO = 0;
};

} // namespace GE