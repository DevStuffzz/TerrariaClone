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
    : shader_(vertexSource, fragmentSource),
      whiteTexture_(1, 1, whitePixel),
      quad_(Mesh::createQuad())
{
    shader_.setInt("uTexture", 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::setViewport(int width, int height) const
{
    if (width < 0 || height < 0) {
        throw std::invalid_argument("Viewport dimensions cannot be negative");
    }
    glViewport(0, 0, width, height);
}

void Renderer::clear(const glm::vec4& color) const
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::beginScene(const Camera2D& camera)
{
    viewProjection_ = camera.viewProjection();
}

void Renderer::setViewProjection(const glm::mat4& viewProjection)
{
    viewProjection_ = viewProjection;
}

void Renderer::draw(const Mesh& mesh, const glm::mat4& transform, const Texture* texture,
                    const glm::vec4& tint) const
{
    shader_.setVec4("uTint", tint);
    (texture ? *texture : whiteTexture_).bind(0);
    draw(mesh, shader_, transform);
}

void Renderer::draw(const Mesh& mesh, const Shader& shader, const glm::mat4& transform) const
{
    shader.setMat4("uViewProjection", viewProjection_);
    shader.setMat4("uTransform", transform);
    mesh.draw();
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size,
                        const glm::vec4& color) const
{
    draw(quad_, transform(position, size), nullptr, color);
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size,
                        const Texture& texture, const glm::vec4& tint) const
{
    draw(quad_, transform(position, size), &texture, tint);
}

glm::mat4 Renderer::transform(const glm::vec2& position, const glm::vec2& size, float rotation)
{
    glm::mat4 result = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
    if (rotation != 0.0f) {
        result = glm::rotate(result, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    return glm::scale(result, glm::vec3(size, 1.0f));
}
