#pragma once

#include "Core/Base.h"

#include <glm/vec2.hpp>

#include <array>

// Values match GLFW key codes so Window can forward events without a lookup table.
enum class Key : int {
    Space = 32,
    Apostrophe = 39, Comma = 44, Minus = 45, Period = 46, Slash = 47,
    D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,
    Semicolon = 59, Equal = 61,
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LeftBracket = 91, Backslash = 92, RightBracket = 93, GraveAccent = 96,
    Escape = 256, Enter, Tab, Backspace, Insert, Delete,
    Right, Left, Down, Up,
    PageUp, PageDown, Home, End,
    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    LeftShift = 340, LeftControl, LeftAlt, LeftSuper,
    RightShift, RightControl, RightAlt, RightSuper
};

enum class MouseButton : int {
    Left = 0,
    Right = 1,
    Middle = 2
};

class ENGINE_API Input {
public:
    // Held down right now.
    bool IsKeyDown(Key key) const;
    // Went down / up since the previous frame.
    bool IsKeyPressed(Key key) const;
    bool IsKeyReleased(Key key) const;

    bool IsMouseButtonDown(MouseButton button) const;
    bool IsMouseButtonPressed(MouseButton button) const;
    bool IsMouseButtonReleased(MouseButton button) const;

    // Window pixel coordinates, origin top-left.
    const glm::vec2& MousePosition() const { return m_mousePosition; }
    // Scroll accumulated since the previous frame.
    const glm::vec2& ScrollDelta() const { return m_scrollDelta; }

    // Called by Window: BeginFrame before polling events, the rest from event callbacks.
    void BeginFrame();
    void OnKey(int key, bool down);
    void OnMouseButton(int button, bool down);
    void OnMouseMove(const glm::vec2& position);
    void OnScroll(const glm::vec2& offset);

private:
    static constexpr int s_keyCount = 349;
    static constexpr int s_mouseButtonCount = 8;

    std::array<bool, s_keyCount> m_keys{};
    std::array<bool, s_keyCount> m_previousKeys{};
    std::array<bool, s_mouseButtonCount> m_buttons{};
    std::array<bool, s_mouseButtonCount> m_previousButtons{};
    glm::vec2 m_mousePosition{0.0f};
    glm::vec2 m_scrollDelta{0.0f};
};
