#include "Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

Camera2D::Camera2D(float viewportWidth, float viewportHeight, float viewHeight)
{
    setViewportSize(viewportWidth, viewportHeight);
    setViewHeight(viewHeight);
}

void Camera2D::setPosition(const glm::vec2& position)
{
    position_ = position;
}

void Camera2D::move(const glm::vec2& delta)
{
    position_ += delta;
}

void Camera2D::setRotation(float radians)
{
    rotation_ = radians;
}

void Camera2D::setViewHeight(float worldUnits)
{
    if (!(worldUnits > 0.0f)) {
        throw std::invalid_argument("Camera view height must be positive");
    }
    viewHeight_ = worldUnits;
}

void Camera2D::setViewportSize(float width, float height)
{
    // Minimized windows report a 0x0 framebuffer; keep the last valid size.
    if (width <= 0.0f || height <= 0.0f) {
        return;
    }
    viewportSize_ = {width, height};
}

glm::vec2 Camera2D::viewSize() const
{
    return {viewHeight_ * viewportSize_.x / viewportSize_.y, viewHeight_};
}

glm::mat4 Camera2D::view() const
{
    glm::mat4 view = glm::rotate(glm::mat4(1.0f), -rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
    return glm::translate(view, glm::vec3(-position_, 0.0f));
}

glm::mat4 Camera2D::projection() const
{
    const glm::vec2 half = viewSize() * 0.5f;
    return glm::ortho(-half.x, half.x, -half.y, half.y, -1.0f, 1.0f);
}

glm::mat4 Camera2D::viewProjection() const
{
    return projection() * view();
}

glm::vec2 Camera2D::screenToWorld(const glm::vec2& screen) const
{
    const glm::vec2 ndc{
        screen.x / viewportSize_.x * 2.0f - 1.0f,
        1.0f - screen.y / viewportSize_.y * 2.0f
    };
    const glm::vec4 world = glm::inverse(viewProjection()) * glm::vec4(ndc, 0.0f, 1.0f);
    return {world.x, world.y};
}
