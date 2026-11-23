#include "Core/Input.h"

namespace {
template <std::size_t N>
bool Get(const std::array<bool, N>& states, int index)
{
    return index >= 0 && index < static_cast<int>(N) && states[static_cast<std::size_t>(index)];
}

template <std::size_t N>
void Set(std::array<bool, N>& states, int index, bool down)
{
    // GLFW reports unknown keys as -1; ignore anything out of range.
    if (index >= 0 && index < static_cast<int>(N)) {
        states[static_cast<std::size_t>(index)] = down;
    }
}
}

bool Input::IsKeyDown(Key key) const
{
    return Get(m_keys, static_cast<int>(key));
}

bool Input::IsKeyPressed(Key key) const
{
    const int index = static_cast<int>(key);
    return Get(m_keys, index) && !Get(m_previousKeys, index);
}

bool Input::IsKeyReleased(Key key) const
{
    const int index = static_cast<int>(key);
    return !Get(m_keys, index) && Get(m_previousKeys, index);
}

bool Input::IsMouseButtonDown(MouseButton button) const
{
    return Get(m_buttons, static_cast<int>(button));
}

bool Input::IsMouseButtonPressed(MouseButton button) const
{
    const int index = static_cast<int>(button);
    return Get(m_buttons, index) && !Get(m_previousButtons, index);
}

bool Input::IsMouseButtonReleased(MouseButton button) const
{
    const int index = static_cast<int>(button);
    return !Get(m_buttons, index) && Get(m_previousButtons, index);
}

void Input::BeginFrame()
{
    m_previousKeys = m_keys;
    m_previousButtons = m_buttons;
    m_scrollDelta = glm::vec2(0.0f);
}

void Input::OnKey(int key, bool down)
{
    Set(m_keys, key, down);
}

void Input::OnMouseButton(int button, bool down)
{
    Set(m_buttons, button, down);
}

void Input::OnMouseMove(const glm::vec2& position)
{
    m_mousePosition = position;
}

void Input::OnScroll(const glm::vec2& offset)
{
    m_scrollDelta += offset;
}
