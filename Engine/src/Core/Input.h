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
    bool isKeyDown(Key key) const;
    // Went down / up since the previous frame.
    bool isKeyPressed(Key key) const;
    bool isKeyReleased(Key key) const;

    bool isMouseButtonDown(MouseButton button) const;
    bool isMouseButtonPressed(MouseButton button) const;
    bool isMouseButtonReleased(MouseButton button) const;

    // Window pixel coordinates, origin top-left.
    const glm::vec2& mousePosition() const { return mousePosition_; }
    // Scroll accumulated since the previous frame.
    const glm::vec2& scrollDelta() const { return scrollDelta_; }

    // Called by Window: beginFrame before polling events, the rest from event callbacks.
    void beginFrame();
    void onKey(int key, bool down);
    void onMouseButton(int button, bool down);
    void onMouseMove(const glm::vec2& position);
    void onScroll(const glm::vec2& offset);

private:
    static constexpr int keyCount = 349;
    static constexpr int mouseButtonCount = 8;

    std::array<bool, keyCount> keys_{};
    std::array<bool, keyCount> previousKeys_{};
    std::array<bool, mouseButtonCount> buttons_{};
    std::array<bool, mouseButtonCount> previousButtons_{};
    glm::vec2 mousePosition_{0.0f};
    glm::vec2 scrollDelta_{0.0f};
};
