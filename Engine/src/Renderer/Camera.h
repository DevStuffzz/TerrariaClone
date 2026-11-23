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

    void SetPosition(const glm::vec2& position);
    void Move(const glm::vec2& delta);
    void SetRotation(float radians);
    void SetViewHeight(float worldUnits);
    void SetViewportSize(float width, float height);

    const glm::vec2& Position() const { return m_position; }
    float Rotation() const { return m_rotation; }
    float ViewHeight() const { return m_viewHeight; }
    glm::vec2 ViewSize() const;

    glm::mat4 View() const;
    glm::mat4 Projection() const;
    glm::mat4 ViewProjection() const;

    // Converts window pixel coordinates (origin top-left, as GLFW reports
    // cursor positions) to world coordinates.
    glm::vec2 ScreenToWorld(const glm::vec2& screen) const;

private:
    glm::vec2 m_position{0.0f};
    float m_rotation = 0.0f;
    float m_viewHeight = 9.0f;
    glm::vec2 m_viewportSize{1.0f};
};
