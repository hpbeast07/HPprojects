#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// IRenderAPI.h — Abstract render backend interface
// Implement this to add Vulkan, DirectX 12, etc. without touching the rest
// ─────────────────────────────────────────────────────────────────────────────
#include "../Math/Math.h"
#include <cstdint>
#include <string>

namespace GE {

enum class RendererBackend {
    None    = 0,
    OpenGL  = 1,
    Vulkan  = 2,
    DirectX12 = 3,
};

struct RenderStats {
    uint32_t drawCalls    = 0;
    uint32_t triangles    = 0;
    uint32_t vertices     = 0;
    float    gpuTimeMs    = 0.0f;
    uint32_t textureCount = 0;
    uint32_t bufferCount  = 0;
};

class IRenderAPI {
public:
    virtual ~IRenderAPI() = default;

    virtual void Init()    = 0;
    virtual void Shutdown() = 0;

    virtual void SetViewport(int x, int y, int width, int height) = 0;
    virtual void SetClearColor(const Vec4& color)                 = 0;
    virtual void Clear()                                          = 0;

    virtual void EnableDepthTest(bool enable) = 0;
    virtual void EnableBlend(bool enable)     = 0;
    virtual void EnableCullFace(bool enable)  = 0;
    virtual void SetWireframe(bool enable)    = 0;

    virtual void SetDepthWrite(bool enable)   = 0;
    virtual void SetCullMode(bool cullFrontFaces) = 0;

    virtual void DrawIndexed(uint32_t indexCount, uint32_t indexOffset = 0) = 0;
    virtual void DrawArrays(uint32_t vertexCount, uint32_t offset = 0)      = 0;

    virtual RendererBackend GetBackend() const = 0;
    virtual std::string     GetInfo()    const = 0;
    virtual RenderStats&    GetStats()         = 0;
    virtual void            ResetStats()       = 0;
};

} // namespace GE
