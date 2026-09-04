#include "OpenGLRenderAPI.h"
#include "../../Core/Logger.h"
#include "../../Core/Assert.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <sstream>

namespace GE {

// ─── OpenGL debug callback ────────────────────────────────────────────────────
static void APIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    // Filter non-critical notifications
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    const char* srcStr = [source] {
        switch (source) {
            case GL_DEBUG_SOURCE_API:             return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WindowSys";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader";
            case GL_DEBUG_SOURCE_THIRD_PARTY:     return "3rdParty";
            case GL_DEBUG_SOURCE_APPLICATION:     return "App";
            default:                              return "Other";
        }
    }();

    const char* typeStr = [type] {
        switch (type) {
            case GL_DEBUG_TYPE_ERROR:               return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UB";
            case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
            default:                                return "Other";
        }
    }();

    if (type == GL_DEBUG_TYPE_ERROR || severity == GL_DEBUG_SEVERITY_HIGH) {
        GE_ERROR("[GL {}|{}] id={} : {}", srcStr, typeStr, id, message);
    } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
        GE_WARN("[GL {}|{}] id={} : {}", srcStr, typeStr, id, message);
    } else {
        GE_DEBUG("[GL {}|{}] id={} : {}", srcStr, typeStr, id, message);
    }
}

// ─── Init ─────────────────────────────────────────────────────────────────────
void OpenGLRenderAPI::Init() {
    // Load OpenGL functions via GLAD
    int gladOk = gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress));
    GE_ASSERT(gladOk, "GLAD failed to load OpenGL functions");

    GE_INFO("OpenGL Info:");
    GE_INFO("  Vendor   : {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    GE_INFO("  Renderer : {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    GE_INFO("  Version  : {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    GE_INFO("  GLSL     : {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    // OpenGL 4.5 is the practical minimum: it supports the DSA calls used by
    // the renderer while remaining compatible with a much wider range of GPUs.
    int major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    GE_ASSERT(major > 4 || (major == 4 && minor >= 5),
        "OpenGL 4.5 or newer is required. Got {}.{}", major, minor);

#ifdef GE_BUILD_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                          0, nullptr, GL_FALSE);
#endif

    // Default state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_MULTISAMPLE);

    // Seamless cubemap for environment maps (Phase 3)
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void OpenGLRenderAPI::Shutdown() {
    GE_INFO("OpenGL backend shutdown");
}

// ─── State ────────────────────────────────────────────────────────────────────
void OpenGLRenderAPI::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGLRenderAPI::SetClearColor(const Vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRenderAPI::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRenderAPI::EnableDepthTest(bool enable) {
    enable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void OpenGLRenderAPI::EnableBlend(bool enable) {
    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void OpenGLRenderAPI::EnableCullFace(bool enable) {
    enable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void OpenGLRenderAPI::SetWireframe(bool enable) {
    glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
}

void OpenGLRenderAPI::SetDepthWrite(bool enable) {
    glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

void OpenGLRenderAPI::SetCullMode(bool cullFrontFaces) {
    glCullFace(cullFrontFaces ? GL_FRONT : GL_BACK);
}

// ─── Draw calls ───────────────────────────────────────────────────────────────
void OpenGLRenderAPI::DrawIndexed(uint32_t indexCount, uint32_t indexOffset) {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                   GL_UNSIGNED_INT,
                   reinterpret_cast<void*>(indexOffset * sizeof(uint32_t)));
    m_Stats.drawCalls++;
    m_Stats.triangles += indexCount / 3;
    m_Stats.vertices += indexCount;
}

void OpenGLRenderAPI::DrawArrays(uint32_t vertexCount, uint32_t offset) {
    glDrawArrays(GL_TRIANGLES, static_cast<GLint>(offset), static_cast<GLsizei>(vertexCount));
    m_Stats.drawCalls++;
    m_Stats.triangles += vertexCount / 3;
    m_Stats.vertices += vertexCount;
}

// ─── Info / Stats ─────────────────────────────────────────────────────────────
std::string OpenGLRenderAPI::GetInfo() const {
    const auto stringOrUnknown = [](GLenum name) {
        const auto* value = reinterpret_cast<const char*>(glGetString(name));
        return value ? value : "unknown";
    };
    std::ostringstream info;
    info << "OpenGL " << stringOrUnknown(GL_VERSION)
         << " | " << stringOrUnknown(GL_VENDOR)
         << " | " << stringOrUnknown(GL_RENDERER);
    return info.str();
}

void OpenGLRenderAPI::ResetStats() {
    m_Stats.drawCalls = 0;
    m_Stats.triangles = 0;
    m_Stats.vertices  = 0;
    m_Stats.gpuTimeMs = 0.0f;
}

} // namespace GE
