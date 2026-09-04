#include "Window.h"
#include "Logger.h"
#include "Assert.h"
#include <GLFW/glfw3.h>

namespace GE {

static void GLFWErrorCallback(int error, const char* description) {
    GE_ERROR("GLFW Error ({}): {}", error, description);
}

Window::Window(const WindowSpec& spec)
    : m_Width(spec.width), m_Height(spec.height), m_VSync(spec.vsync)
{
    // Initialize GLFW (safe to call multiple times)
    static bool s_GLFWInitialized = false;
    if (!s_GLFWInitialized) {
        int ok = glfwInit();
        GE_ASSERT(ok, "Failed to initialize GLFW");
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
        GE_INFO("GLFW initialized (version {})", glfwGetVersionString());
    }

    // Request OpenGL 4.5 Core Profile. The renderer uses DSA APIs introduced in
    // 4.5, and all engine-owned GLSL sources target GLSL 4.50.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, spec.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA 4x

#ifdef GE_BUILD_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    m_Window = glfwCreateWindow(m_Width, m_Height, spec.title.c_str(), nullptr, nullptr);
    GE_ASSERT(m_Window, "Failed to create GLFW window");

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(m_VSync ? 1 : 0);

    // Store 'this' so static callbacks can access the Window instance
    glfwSetWindowUserPointer(m_Window, this);
    SetupCallbacks();

    GE_INFO("Window created: {}x{} \"{}\"", m_Width, m_Height, spec.title);
}

Window::~Window() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
    // Note: glfwTerminate() is called by Application on full shutdown
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::SwapBuffers() {
    glfwSwapBuffers(m_Window);
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Window::SetShouldClose(bool value) {
    glfwSetWindowShouldClose(m_Window, value ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SetVSync(bool enabled) {
    m_VSync = enabled;
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(m_VSync ? 1 : 0);
}

void Window::SetupCallbacks() {
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* w, int width, int height) {
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!win) return;
        win->m_Width  = width;
        win->m_Height = height;
        if (win->OnResize) win->OnResize(width, height);
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!win) return;
        if (win->OnKey) win->OnKey(key, scancode, action, mods);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* w, double x, double y) {
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!win) return;
        if (win->OnMouseMove) win->OnMouseMove(x, y);
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* w, int button, int action, int mods) {
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!win) return;
        if (win->OnMouseButton) win->OnMouseButton(button, action, mods);
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* w, double xoff, double yoff) {
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!win) return;
        if (win->OnMouseScroll) win->OnMouseScroll(xoff, yoff);
    });

    glfwSetJoystickCallback([](int jid, int event) {
        GE_INFO("Joystick {}: {}", jid, (event == GLFW_CONNECTED) ? "connected" : "disconnected");
    });
}

} // namespace GE
