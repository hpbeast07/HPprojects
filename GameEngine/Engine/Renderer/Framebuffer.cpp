#include "Framebuffer.h"
#include "../Core/Logger.h"
#include "../Core/Assert.h"

#include <algorithm>

namespace GE {

Framebuffer::Framebuffer(const Spec& spec) : m_Spec(spec) {
    Invalidate();
}

Framebuffer::~Framebuffer() {
    if (m_ID) glDeleteFramebuffers(1, &m_ID);
    if (m_DepthStencilRBO) glDeleteRenderbuffers(1, &m_DepthStencilRBO);
}

void Framebuffer::Bind() const {
    if (!m_ID) return;
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    glViewport(0, 0, static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height));
}

void Framebuffer::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ─── Invalidate: (re)create all attachments ─────────────────────────────────
void Framebuffer::Invalidate() {
    m_Width  = std::max(1u, m_Spec.width);
    m_Height = std::max(1u, m_Spec.height);

    if (m_ID) glDeleteFramebuffers(1, &m_ID);
    if (m_DepthStencilRBO) glDeleteRenderbuffers(1, &m_DepthStencilRBO);
    m_ColorAttachments.clear();

    glCreateFramebuffers(1, &m_ID);

    // Color attachment
    auto color = std::make_shared<Texture2D>(m_Width, m_Height, m_Spec.colorInternal);
    color->SetFilter(TextureFilter::Linear);
    color->SetWrap(TextureWrap::Clamp);
    m_ColorAttachments.push_back(color);
    glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0, color->GetID(), 0);

    // Depth (and optionally stencil) attachment
    if (m_Spec.enableDepth) {
        const GLenum depthFormat = m_Spec.enableStencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
        glCreateRenderbuffers(1, &m_DepthStencilRBO);
        glNamedRenderbufferStorage(m_DepthStencilRBO, depthFormat, m_Width, m_Height);
        const GLenum attachment = m_Spec.enableStencil
            ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
        glNamedFramebufferRenderbuffer(m_ID, attachment, GL_RENDERBUFFER, m_DepthStencilRBO);
    }

    const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glNamedFramebufferDrawBuffers(m_ID, 1, drawBuffers);

    const GLenum status = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
    GE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE,
        "Framebuffer incomplete! status={:#x}", static_cast<uint32_t>(status));
}

void Framebuffer::Resize(uint32_t width, uint32_t height) {
    // Guard against degenerate sizes (minimized windows, zero-area docks)
    if (width < 1)  width  = 1;
    if (height < 1) height = 1;
    m_Spec.width  = width;
    m_Spec.height = height;
    Invalidate();
}

void Framebuffer::ResizeChecked(uint32_t width, uint32_t height) {
    if (width == m_Width && height == m_Height) return;
    Resize(width, height);
}

std::shared_ptr<Texture2D> Framebuffer::GetColorAttachment(size_t index) const {
    if (index >= m_ColorAttachments.size()) return nullptr;
    return m_ColorAttachments[index];
}

} // namespace GE