#include "Core/Input.h"

namespace {
template <std::size_t N>
bool get(const std::array<bool, N>& states, int index)
{
    return index >= 0 && index < static_cast<int>(N) && states[static_cast<std::size_t>(index)];
}

template <std::size_t N>
void set(std::array<bool, N>& states, int index, bool down)
{
    // GLFW reports unknown keys as -1; ignore anything out of range.
    if (index >= 0 && index < static_cast<int>(N)) {
        states[static_cast<std::size_t>(index)] = down;
    }
}
}

bool Input::isKeyDown(Key key) const
{
    return get(keys_, static_cast<int>(key));
}

bool Input::isKeyPressed(Key key) const
{
    const int index = static_cast<int>(key);
    return get(keys_, index) && !get(previousKeys_, index);
}

bool Input::isKeyReleased(Key key) const
{
    const int index = static_cast<int>(key);
    return !get(keys_, index) && get(previousKeys_, index);
}

bool Input::isMouseButtonDown(MouseButton button) const
{
    return get(buttons_, static_cast<int>(button));
}

bool Input::isMouseButtonPressed(MouseButton button) const
{
    const int index = static_cast<int>(button);
    return get(buttons_, index) && !get(previousButtons_, index);
}

bool Input::isMouseButtonReleased(MouseButton button) const
{
    const int index = static_cast<int>(button);
    return !get(buttons_, index) && get(previousButtons_, index);
}

void Input::beginFrame()
{
    previousKeys_ = keys_;
    previousButtons_ = buttons_;
    scrollDelta_ = glm::vec2(0.0f);
}

void Input::onKey(int key, bool down)
{
    set(keys_, key, down);
}

void Input::onMouseButton(int button, bool down)
{
    set(buttons_, button, down);
}

void Input::onMouseMove(const glm::vec2& position)
{
    mousePosition_ = position;
}

void Input::onScroll(const glm::vec2& offset)
{
    scrollDelta_ += offset;
}
