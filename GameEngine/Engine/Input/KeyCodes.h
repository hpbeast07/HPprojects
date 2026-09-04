#pragma once
#include <GLFW/glfw3.h>
#include <array>

namespace GE {

// ─────────────────────────────────────────────────────────────────────────────
// KeyCode — mirrors GLFW key codes for portability
// ─────────────────────────────────────────────────────────────────────────────
enum class KeyCode : int {
    // Printable
    Space        = GLFW_KEY_SPACE,
    Apostrophe   = GLFW_KEY_APOSTROPHE,
    Comma        = GLFW_KEY_COMMA,
    Minus        = GLFW_KEY_MINUS,
    Period       = GLFW_KEY_PERIOD,
    Slash        = GLFW_KEY_SLASH,
    D0 = GLFW_KEY_0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
    Semicolon    = GLFW_KEY_SEMICOLON,
    Equal        = GLFW_KEY_EQUAL,
    A = GLFW_KEY_A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    // Function
    Escape       = GLFW_KEY_ESCAPE,
    Enter        = GLFW_KEY_ENTER,
    Tab          = GLFW_KEY_TAB,
    Backspace    = GLFW_KEY_BACKSPACE,
    Insert       = GLFW_KEY_INSERT,
    Delete       = GLFW_KEY_DELETE,
    Right        = GLFW_KEY_RIGHT,
    Left         = GLFW_KEY_LEFT,
    Down         = GLFW_KEY_DOWN,
    Up           = GLFW_KEY_UP,
    PageUp       = GLFW_KEY_PAGE_UP,
    PageDown     = GLFW_KEY_PAGE_DOWN,
    Home         = GLFW_KEY_HOME,
    End          = GLFW_KEY_END,
    CapsLock     = GLFW_KEY_CAPS_LOCK,
    F1  = GLFW_KEY_F1,  F2,  F3,  F4,  F5,  F6,
    F7,  F8,  F9,  F10, F11, F12,
    LeftShift    = GLFW_KEY_LEFT_SHIFT,
    LeftControl  = GLFW_KEY_LEFT_CONTROL,
    LeftAlt      = GLFW_KEY_LEFT_ALT,
    LeftSuper    = GLFW_KEY_LEFT_SUPER,
    RightShift   = GLFW_KEY_RIGHT_SHIFT,
    RightControl = GLFW_KEY_RIGHT_CONTROL,
    RightAlt     = GLFW_KEY_RIGHT_ALT,
    RightSuper   = GLFW_KEY_RIGHT_SUPER,
    // Numpad
    KP0 = GLFW_KEY_KP_0, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
    KPDecimal  = GLFW_KEY_KP_DECIMAL,
    KPDivide   = GLFW_KEY_KP_DIVIDE,
    KPMultiply = GLFW_KEY_KP_MULTIPLY,
    KPSubtract = GLFW_KEY_KP_SUBTRACT,
    KPAdd      = GLFW_KEY_KP_ADD,
    KPEnter    = GLFW_KEY_KP_ENTER,
};

enum class MouseButton : int {
    Left   = GLFW_MOUSE_BUTTON_LEFT,
    Right  = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    B4     = GLFW_MOUSE_BUTTON_4,
    B5     = GLFW_MOUSE_BUTTON_5,
};

} // namespace GE
