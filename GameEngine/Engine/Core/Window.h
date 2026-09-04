#pragma once
#include <functional>
#include <string>

struct GLFWwindow; // forward decl at global scope — matches GLFW's real symbol

namespace GE {

struct WindowSpec {
    std::string title  = "GameEngine";
    int         width  = 1600;
    int         height = 900;
    bool        vsync  = true;
    bool        resizable = true;
};

class Window {
public:
    explicit Window(const WindowSpec& spec);
    ~Window();

    // Non-copyable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void PollEvents();
    void SwapBuffers();
    bool ShouldClose() const;
    void SetShouldClose(bool value);
    void SetVSync(bool enabled);

    int  GetWidth()  const { return m_Width;  }
    int  GetHeight() const { return m_Height; }
    float GetAspectRatio() const { return m_Height > 0 ? static_cast<float>(m_Width) / static_cast<float>(m_Height) : 1.0f; }

    GLFWwindow* GetNativeWindow() const { return m_Window; }
    bool IsVSync() const { return m_VSync; }

    // Callbacks (set by Application)
    std::function<void(int,int)>          OnResize;
    std::function<void(int,int,int,int)>  OnKey;
    std::function<void(double,double)>    OnMouseMove;
    std::function<void(int,int,int)>      OnMouseButton;
    std::function<void(double,double)>    OnMouseScroll;
    std::function<void(int,int,int,int)>  OnJoystick;

private:
    void SetupCallbacks();

    GLFWwindow* m_Window = nullptr;
    int         m_Width;
    int         m_Height;
    bool        m_VSync;
};

} // namespace GE
