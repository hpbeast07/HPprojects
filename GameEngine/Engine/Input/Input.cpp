#include "Input.h"
#include <GLFW/glfw3.h>

namespace GE {

// ─── Static member definitions ────────────────────────────────────────────────
GLFWwindow*                                   Input::s_Window       = nullptr;
std::array<bool, Input::k_MaxKeys>            Input::s_Keys         = {};
std::array<bool, Input::k_MaxKeys>            Input::s_PrevKeys     = {};
std::array<bool, Input::k_MaxButtons>         Input::s_MouseButtons = {};
std::array<bool, Input::k_MaxButtons>         Input::s_PrevMouseButtons = {};
Vec2                                          Input::s_MousePos     = {0.0f, 0.0f};
Vec2                                          Input::s_PrevMousePos = {0.0f, 0.0f};
Vec2                                          Input::s_MouseDelta   = {0.0f, 0.0f};
float                                         Input::s_ScrollDelta  = 0.0f;
float                                         Input::s_ScrollAccum  = 0.0f;
bool                                          Input::s_CursorLocked = false;
bool                                          Input::s_FirstMouse   = true;

// ─── Init ─────────────────────────────────────────────────────────────────────
void Input::Init(GLFWwindow* window) {
    s_Window = window;
    s_Keys.fill(false);
    s_PrevKeys.fill(false);
    s_MouseButtons.fill(false);
    s_PrevMouseButtons.fill(false);
    s_MousePos = { 0.0f, 0.0f };
    s_PrevMousePos = { 0.0f, 0.0f };
    s_MouseDelta = { 0.0f, 0.0f };
    s_ScrollDelta = 0.0f;
    s_ScrollAccum = 0.0f;
    s_CursorLocked = false;
    s_FirstMouse = true;
}

// ─── Update ───────────────────────────────────────────────────────────────────
void Input::Update() {
    // Copy current → previous
    s_PrevKeys         = s_Keys;
    s_PrevMouseButtons = s_MouseButtons;

    // Consume accumulated scroll
    s_ScrollDelta = s_ScrollAccum;
    s_ScrollAccum = 0.0f;

    // Mouse movement is accumulated by callbacks and consumed once per frame.
    s_MouseDelta = { 0.0f, 0.0f };
}

// ─── Keyboard ─────────────────────────────────────────────────────────────────
bool Input::IsKeyDown(KeyCode key) {
    int k = static_cast<int>(key);
    if (k < 0 || k >= k_MaxKeys) return false;
    return s_Keys[k];
}

bool Input::IsKeyUp(KeyCode key) {
    return !IsKeyDown(key);
}

bool Input::IsKeyPressed(KeyCode key) {
    int k = static_cast<int>(key);
    if (k < 0 || k >= k_MaxKeys) return false;
    return s_Keys[k] && !s_PrevKeys[k];
}

bool Input::IsKeyReleased(KeyCode key) {
    int k = static_cast<int>(key);
    if (k < 0 || k >= k_MaxKeys) return false;
    return !s_Keys[k] && s_PrevKeys[k];
}

// ─── Mouse buttons ────────────────────────────────────────────────────────────
bool Input::IsMouseButtonDown(MouseButton btn) {
    int b = static_cast<int>(btn);
    if (b < 0 || b >= k_MaxButtons) return false;
    return s_MouseButtons[b];
}

bool Input::IsMouseButtonPressed(MouseButton btn) {
    int b = static_cast<int>(btn);
    if (b < 0 || b >= k_MaxButtons) return false;
    return s_MouseButtons[b] && !s_PrevMouseButtons[b];
}

bool Input::IsMouseButtonReleased(MouseButton btn) {
    int b = static_cast<int>(btn);
    if (b < 0 || b >= k_MaxButtons) return false;
    return !s_MouseButtons[b] && s_PrevMouseButtons[b];
}

// ─── Mouse position/delta ─────────────────────────────────────────────────────
Vec2 Input::GetMousePosition() { return s_MousePos; }
Vec2 Input::GetMouseDelta()    { return s_MouseDelta; }
float Input::GetScrollDelta()  { return s_ScrollDelta; }

// ─── Cursor lock ──────────────────────────────────────────────────────────────
void Input::SetCursorLocked(bool locked) {
    s_CursorLocked = locked;
    if (s_Window) {
        glfwSetInputMode(s_Window, GLFW_CURSOR,
            locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    s_FirstMouse = true; // Reset delta on mode change
}

bool Input::IsCursorLocked() { return s_CursorLocked; }

// ─── Event callbacks (called by Window) ──────────────────────────────────────
void Input::OnKeyEvent(int key, int action) {
    if (key >= 0 && key < k_MaxKeys) {
        s_Keys[key] = (action != GLFW_RELEASE);
    }
}

void Input::OnMouseButtonEvent(int button, int action) {
    if (button >= 0 && button < k_MaxButtons) {
        s_MouseButtons[button] = (action != GLFW_RELEASE);
    }
}

void Input::OnMouseMoveEvent(double x, double y) {
    const Vec2 newPosition(static_cast<float>(x), static_cast<float>(y));
    if (s_FirstMouse) {
        s_MousePos = newPosition;
        s_PrevMousePos = newPosition;
        s_FirstMouse   = false;
        return;
    }
    s_MouseDelta += newPosition - s_MousePos;
    s_PrevMousePos = s_MousePos;
    s_MousePos = newPosition;
}

void Input::OnScrollEvent(double yoffset) {
    s_ScrollAccum += static_cast<float>(yoffset);
}

} // namespace GE
