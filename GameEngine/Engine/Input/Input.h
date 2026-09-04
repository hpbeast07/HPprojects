#pragma once
#include "KeyCodes.h"
#include "../Math/Math.h"
#include <array>

// Forward declaration
struct GLFWwindow;

namespace GE {

// ─────────────────────────────────────────────────────────────────────────────
// Input — immediate-mode query API (also tracks prev-frame state for IsJustPressed)
// ─────────────────────────────────────────────────────────────────────────────
class Input {
public:
    static void Init(GLFWwindow* window);
    static void Update();           // Call once per frame BEFORE PollEvents

    // Keyboard
    static bool IsKeyDown(KeyCode key);
    static bool IsKeyUp(KeyCode key);
    static bool IsKeyPressed(KeyCode key);   // down this frame, not last frame
    static bool IsKeyReleased(KeyCode key);  // up this frame, was down last frame

    // Mouse buttons
    static bool IsMouseButtonDown(MouseButton btn);
    static bool IsMouseButtonPressed(MouseButton btn);
    static bool IsMouseButtonReleased(MouseButton btn);

    // Mouse position / delta
    static Vec2 GetMousePosition();
    static Vec2 GetMouseDelta();
    static float GetScrollDelta();

    // Cursor lock (for FPS camera)
    static void SetCursorLocked(bool locked);
    static bool IsCursorLocked();

    // Called by Window callbacks — do not call directly
    static void OnKeyEvent(int key, int action);
    static void OnMouseButtonEvent(int button, int action);
    static void OnMouseMoveEvent(double x, double y);
    static void OnScrollEvent(double yoffset);

private:
    static constexpr int k_MaxKeys    = 512;
    static constexpr int k_MaxButtons = 8;

    static GLFWwindow*          s_Window;
    static std::array<bool, k_MaxKeys>    s_Keys;
    static std::array<bool, k_MaxKeys>    s_PrevKeys;
    static std::array<bool, k_MaxButtons> s_MouseButtons;
    static std::array<bool, k_MaxButtons> s_PrevMouseButtons;
    static Vec2  s_MousePos;
    static Vec2  s_PrevMousePos;
    static Vec2  s_MouseDelta;
    static float s_ScrollDelta;
    static float s_ScrollAccum;
    static bool  s_CursorLocked;
    static bool  s_FirstMouse;
};

} // namespace GE
