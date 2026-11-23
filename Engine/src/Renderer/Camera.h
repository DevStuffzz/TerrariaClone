#pragma once

#include "Core/Base.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

// Orthographic 2D camera. `position` is the world point at the center of the
// screen and `viewHeight` is how many world units fit vertically in the view;
// the visible width follows the viewport aspect ratio.
class ENGINE_API Camera2D {
public:
    Camera2D(float viewportWidth, float viewportHeight, float viewHeight = 9.0f);

    void setPosition(const glm::vec2& position);
    void move(const glm::vec2& delta);
    void setRotation(float radians);
    void setViewHeight(float worldUnits);
    void setViewportSize(float width, float height);

    const glm::vec2& position() const { return position_; }
    float rotation() const { return rotation_; }
    float viewHeight() const { return viewHeight_; }
    glm::vec2 viewSize() const;

    glm::mat4 view() const;
    glm::mat4 projection() const;
    glm::mat4 viewProjection() const;

    // Converts window pixel coordinates (origin top-left, as GLFW reports
    // cursor positions) to world coordinates.
    glm::vec2 screenToWorld(const glm::vec2& screen) const;

private:
    glm::vec2 position_{0.0f};
    float rotation_ = 0.0f;
    float viewHeight_ = 9.0f;
    glm::vec2 viewportSize_{1.0f};
};
