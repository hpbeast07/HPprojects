#pragma once
#include "../IRenderAPI.h"

namespace GE {

class OpenGLRenderAPI final : public IRenderAPI {
public:
    void Init()     override;
    void Shutdown() override;

    void SetViewport(int x, int y, int width, int height) override;
    void SetClearColor(const Vec4& color) override;
    void Clear()    override;

    void EnableDepthTest(bool enable) override;
    void EnableBlend(bool enable)     override;
    void EnableCullFace(bool enable)  override;
    void SetWireframe(bool enable)    override;
    void SetDepthWrite(bool enable)   override;
    void SetCullMode(bool cullFrontFaces) override;

    void DrawIndexed(uint32_t indexCount, uint32_t indexOffset = 0) override;
    void DrawArrays(uint32_t vertexCount, uint32_t offset = 0)      override;

    RendererBackend GetBackend() const override { return RendererBackend::OpenGL; }
    std::string     GetInfo()    const override;
    RenderStats&    GetStats()         override { return m_Stats; }
    void            ResetStats()       override;

private:
    RenderStats m_Stats;
};

} // namespace GE
