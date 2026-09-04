#include "Application.h"
#include "Logger.h"
#include "Assert.h"
#include "../Renderer/OpenGL/OpenGLRenderAPI.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <algorithm>

namespace GE {

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpec& spec) : m_Spec(spec) {
    GE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    // ── Logger setup ──────────────────────────────────────────────────────────
    Logger::Get().SetLevel(LogLevel::Trace);
    if (!spec.logFile.empty())
        Logger::Get().SetLogFile(spec.logFile);

    GE_INFO("=== {} starting ===", spec.name);
    GE_INFO("Built: " __DATE__ " " __TIME__);

    // ── Window ────────────────────────────────────────────────────────────────
    WindowSpec ws;
    ws.title  = spec.name;
    ws.width  = spec.width;
    ws.height = spec.height;
    ws.vsync  = spec.vsync;
    m_Window  = std::make_unique<Window>(ws);

    // Wire window callbacks → Input system
    m_Window->OnKey = [](int key, int /*scan*/, int action, int /*mods*/) {
        Input::OnKeyEvent(key, action);
    };
    m_Window->OnMouseMove = [](double x, double y) {
        Input::OnMouseMoveEvent(x, y);
    };
    m_Window->OnMouseButton = [](int btn, int action, int /*mods*/) {
        Input::OnMouseButtonEvent(btn, action);
    };
    m_Window->OnMouseScroll = [](double /*x*/, double y) {
        Input::OnScrollEvent(y);
    };
    m_Window->OnResize = [this](int w, int h) {
        if (m_RenderAPI && w > 0 && h > 0)
            m_RenderAPI->SetViewport(0, 0, w, h);
        GE_INFO("Window resized: {}x{}", w, h);
    };

    // ── Input ─────────────────────────────────────────────────────────────────
    Input::Init(m_Window->GetNativeWindow());

    // ── Render API ────────────────────────────────────────────────────────────
    m_RenderAPI = std::make_unique<OpenGLRenderAPI>();
    m_RenderAPI->Init();
    m_RenderAPI->SetViewport(0, 0, spec.width, spec.height);

    // ── ImGui ─────────────────────────────────────────────────────────────────
    if (m_Spec.enableImGui)
        InitImGui();

    OnInit();
}

Application::~Application() {
    OnShutdown();

    // Detach all layers
    for (auto& layer : m_Layers) layer->OnDetach();
    m_Layers.clear();

    // ImGui and all layer-owned GL resources must be released while the main
    // window's OpenGL context is still current.
    if (m_ImGuiInitialized)
        ShutdownImGui();
    if (m_RenderAPI)
        m_RenderAPI->Shutdown();

    // m_Window destructor handles GLFW window destruction
    m_Window.reset();
    glfwTerminate();

    GE_INFO("=== Application shutdown ===");
    s_Instance = nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Main Loop
// ─────────────────────────────────────────────────────────────────────────────
void Application::Run() {
    auto lastTime = std::chrono::high_resolution_clock::now();

    GE_INFO("Entering main loop");

    while (m_Running && !m_Window->ShouldClose()) {
        // ── Timing ─────────────────────────────────────────────────────────
        auto now     = std::chrono::high_resolution_clock::now();
        float rawDt  = std::chrono::duration<float>(now - lastTime).count();
        lastTime     = now;
        m_DeltaTime  = std::min(rawDt, k_MaxDeltaTime);
        m_Time      += m_DeltaTime;

        // ── Input ──────────────────────────────────────────────────────────
        Input::Update();        // copy prev-frame state
        m_Window->PollEvents(); // fires GLFW callbacks → Input

        // ESC to close (overridable by layers)
        if (Input::IsKeyPressed(KeyCode::Escape))
            Close();

        // ── Fixed update ───────────────────────────────────────────────────
        m_FixedAccum += m_DeltaTime;
        while (m_FixedAccum >= k_FixedTimestep) {
            for (auto& layer : m_Layers)
                layer->OnFixedUpdate(k_FixedTimestep);
            m_FixedAccum -= k_FixedTimestep;
        }

        // ── Variable update ────────────────────────────────────────────────
        m_RenderAPI->ResetStats();
        for (auto& layer : m_Layers)
            layer->OnUpdate(m_DeltaTime);

        // ── ImGui ──────────────────────────────────────────────────────────
        if (m_ImGuiInitialized) {
            BeginImGuiFrame();
            for (auto& layer : m_Layers)
                layer->OnImGuiRender();
            EndImGuiFrame();
        }

        // ── Present ────────────────────────────────────────────────────────
        m_Window->SwapBuffers();
        UpdateFPS();
    }
    GE_INFO("Main loop exited");
}

void Application::Close() {
    m_Running = false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Layer management
// ─────────────────────────────────────────────────────────────────────────────
void Application::PushLayer(std::shared_ptr<Layer> layer) {
    GE_ASSERT(layer, "Cannot push a null layer");
    layer->OnAttach();
    m_Layers.push_back(std::move(layer));
}

void Application::PopLayer(std::shared_ptr<Layer> layer) {
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if (it != m_Layers.end()) {
        (*it)->OnDetach();
        m_Layers.erase(it);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// ImGui
// ─────────────────────────────────────────────────────────────────────────────
void Application::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding   = 4.0f;
    style.FrameRounding    = 3.0f;
    style.PopupRounding    = 3.0f;
    style.ScrollbarRounding= 3.0f;
    style.GrabRounding     = 3.0f;
    style.TabRounding      = 3.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_Window->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
    m_ImGuiInitialized = true;
    GE_INFO("ImGui initialized ({})", IMGUI_VERSION);
}

void Application::ShutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_ImGuiInitialized = false;
}

void Application::BeginImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Application::EndImGuiFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Multi-viewport support
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// FPS calculation
// ─────────────────────────────────────────────────────────────────────────────
void Application::UpdateFPS() {
    m_FrameCount++;
    m_FPSTimer += m_DeltaTime;
    if (m_FPSTimer >= 0.5f) {
        m_FPS        = static_cast<int>(m_FrameCount / m_FPSTimer);
        m_FrameCount = 0;
        m_FPSTimer   = 0.0f;
    }
}

} // namespace GE
