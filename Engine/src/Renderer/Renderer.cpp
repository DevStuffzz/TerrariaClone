#include "Renderer/Renderer.h"

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

namespace {
const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;
uniform mat4 uViewProjection;
uniform mat4 uTransform;
out vec2 vTexCoord;
out vec4 vColor;
void main()
{
    vTexCoord = aTexCoord;
    vColor = aColor;
    gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);
}
)";

const char* fragmentSource = R"(
#version 330 core
in vec2 vTexCoord;
in vec4 vColor;
out vec4 fragmentColor;
uniform sampler2D uTexture;
uniform vec4 uTint;
void main()
{
    fragmentColor = texture(uTexture, vTexCoord) * vColor * uTint;
}
)";

const unsigned char whitePixel[] = {255, 255, 255, 255};
}

Renderer::Renderer()
    : m_shader(vertexSource, fragmentSource),
      m_whiteTexture(1, 1, whitePixel),
      m_quad(Mesh::CreateQuad())
{
    m_shader.SetInt("uTexture", 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::SetViewport(int width, int height) const
{
    if (width < 0 || height < 0) {
        throw std::invalid_argument("Viewport dimensions cannot be negative");
    }
    glViewport(0, 0, width, height);
}

void Renderer::Clear(const glm::vec4& color) const
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::BeginScene(const Camera2D& camera)
{
    m_viewProjection = camera.ViewProjection();
}

void Renderer::SetViewProjection(const glm::mat4& viewProjection)
{
    m_viewProjection = viewProjection;
}

void Renderer::Draw(const Mesh& mesh, const glm::mat4& transform, const Texture* texture,
                    const glm::vec4& tint) const
{
    m_shader.SetVec4("uTint", tint);
    (texture ? *texture : m_whiteTexture).Bind(0);
    Draw(mesh, m_shader, transform);
}

void Renderer::Draw(const Mesh& mesh, const Shader& shader, const glm::mat4& transform) const
{
    shader.SetMat4("uViewProjection", m_viewProjection);
    shader.SetMat4("uTransform", transform);
    mesh.Draw();
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                        const glm::vec4& color) const
{
    Draw(m_quad, Transform(position, size), nullptr, color);
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                        const Texture& texture, const glm::vec4& tint) const
{
    Draw(m_quad, Transform(position, size), &texture, tint);
}

glm::mat4 Renderer::Transform(const glm::vec2& position, const glm::vec2& size, float rotation)
{
    glm::mat4 result = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
    if (rotation != 0.0f) {
        result = glm::rotate(result, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    return glm::scale(result, glm::vec3(size, 1.0f));
}
