#include "Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

Camera2D::Camera2D(float viewportWidth, float viewportHeight, float viewHeight)
{
    SetViewportSize(viewportWidth, viewportHeight);
    SetViewHeight(viewHeight);
}

void Camera2D::SetPosition(const glm::vec2& position)
{
    m_position = position;
}

void Camera2D::Move(const glm::vec2& delta)
{
    m_position += delta;
}

void Camera2D::SetRotation(float radians)
{
    m_rotation = radians;
}

void Camera2D::SetViewHeight(float worldUnits)
{
    if (!(worldUnits > 0.0f)) {
        throw std::invalid_argument("Camera view height must be positive");
    }
    m_viewHeight = worldUnits;
}

void Camera2D::SetViewportSize(float width, float height)
{
    // Minimized windows report a 0x0 framebuffer; keep the last valid size.
    if (width <= 0.0f || height <= 0.0f) {
        return;
    }
    m_viewportSize = {width, height};
}

glm::vec2 Camera2D::ViewSize() const
{
    return {m_viewHeight * m_viewportSize.x / m_viewportSize.y, m_viewHeight};
}

glm::mat4 Camera2D::View() const
{
    glm::mat4 view = glm::rotate(glm::mat4(1.0f), -m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    return glm::translate(view, glm::vec3(-m_position, 0.0f));
}

glm::mat4 Camera2D::Projection() const
{
    const glm::vec2 half = ViewSize() * 0.5f;
    return glm::ortho(-half.x, half.x, -half.y, half.y, -1.0f, 1.0f);
}

glm::mat4 Camera2D::ViewProjection() const
{
    return Projection() * View();
}

glm::vec2 Camera2D::ScreenToWorld(const glm::vec2& screen) const
{
    const glm::vec2 ndc{
        screen.x / m_viewportSize.x * 2.0f - 1.0f,
        1.0f - screen.y / m_viewportSize.y * 2.0f
    };
    const glm::vec4 world = glm::inverse(ViewProjection()) * glm::vec4(ndc, 0.0f, 1.0f);
    return {world.x, world.y};
}
