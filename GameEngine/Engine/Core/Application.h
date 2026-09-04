#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Application.h — Engine lifecycle and main loop orchestrator
// ─────────────────────────────────────────────────────────────────────────────
#include "Window.h"
#include "../Input/Input.h"
#include "../Renderer/IRenderAPI.h"
#include <memory>
#include <string>
#include <chrono>
#include <vector>

namespace GE {

struct ApplicationSpec {
    std::string name    = "GameEngine Application";
    int         width   = 1600;
    int         height  = 900;
    bool        vsync   = true;
    bool        enableImGui = true;
    std::string logFile = "engine.log";
};

// ─────────────────────────────────────────────────────────────────────────────
// Layer — base class for editor, game, and debug layers
// ─────────────────────────────────────────────────────────────────────────────
class Layer {
public:
    explicit Layer(std::string name) : m_Name(std::move(name)) {}
    virtual ~Layer() = default;

    virtual void OnAttach()                   {}
    virtual void OnDetach()                   {}
    virtual void OnUpdate(float deltaTime)    {}
    virtual void OnFixedUpdate(float fixedDt) {}
    virtual void OnImGuiRender()              {}
    virtual void OnEvent(int key, int action) {}

    const std::string& GetName() const { return m_Name; }

protected:
    std::string m_Name;
};

// ─────────────────────────────────────────────────────────────────────────────
// Application — singleton engine host
// ─────────────────────────────────────────────────────────────────────────────
class Application {
public:
    explicit Application(const ApplicationSpec& spec);
    virtual ~Application();

    // Non-copyable singleton
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Run();
    void Close();

    void PushLayer(std::shared_ptr<Layer> layer);
    void PopLayer(std::shared_ptr<Layer> layer);

    Window&      GetWindow()     { return *m_Window; }
    IRenderAPI&  GetRenderAPI()  { return *m_RenderAPI; }
    const ApplicationSpec& GetSpecification() const { return m_Spec; }

    float GetDeltaTime()    const { return m_DeltaTime; }
    float GetTime()         const { return m_Time; }
    int   GetFPS()          const { return m_FPS; }
    float GetFrameTimeMs()  const { return m_DeltaTime * 1000.0f; }

    static Application& Get() { return *s_Instance; }

protected:
    // Override in subclasses (Editor, Runtime) for custom init
    virtual void OnInit()     {}
    virtual void OnShutdown() {}

private:
    void InitImGui();
    void ShutdownImGui();
    void BeginImGuiFrame();
    void EndImGuiFrame();
    void UpdateFPS();

    ApplicationSpec                       m_Spec;
    std::unique_ptr<Window>               m_Window;
    std::unique_ptr<IRenderAPI>           m_RenderAPI;
    std::vector<std::shared_ptr<Layer>>   m_Layers;

    bool  m_Running      = true;
    bool  m_ImGuiInitialized = false;
    float m_DeltaTime    = 0.0f;
    float m_Time         = 0.0f;
    float m_FixedAccum   = 0.0f;
    float m_FPSTimer     = 0.0f;
    int   m_FrameCount   = 0;
    int   m_FPS          = 0;

    static constexpr float k_FixedTimestep = 1.0f / 60.0f;
    static constexpr float k_MaxDeltaTime  = 0.05f; // Cap at 50ms to avoid spiral of death

    static Application* s_Instance;
};

} // namespace GE
