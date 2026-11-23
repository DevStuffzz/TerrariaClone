#pragma once

#include "Core/Base.h"

#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class ENGINE_API Renderer {
public:
    Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void SetViewport(int width, int height) const;
    void Clear(const glm::vec4& color) const;

    void BeginScene(const Camera2D& camera);
    void SetViewProjection(const glm::mat4& viewProjection);

    // Draws with the built-in shader. A null texture draws untextured; the
    // final color is texture * vertex color * tint.
    void Draw(const Mesh& mesh, const glm::mat4& transform = glm::mat4(1.0f),
              const Texture* texture = nullptr, const glm::vec4& tint = glm::vec4(1.0f)) const;
    // Draws with a custom shader. The renderer sets `uViewProjection` and
    // `uTransform`; set any other uniforms on the shader before calling.
    void Draw(const Mesh& mesh, const Shader& shader,
              const glm::mat4& transform = glm::mat4(1.0f)) const;

    // Quad positions are the lower-left corner in world units.
    void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                  const glm::vec4& color) const;
    void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                  const Texture& texture, const glm::vec4& tint = glm::vec4(1.0f)) const;

    // Builds a model matrix that scales by `size`, rotates around the
    // origin, then translates to `position`.
    static glm::mat4 Transform(const glm::vec2& position, const glm::vec2& size = glm::vec2(1.0f),
                               float rotation = 0.0f);

    const Shader& GetDefaultShader() const { return m_shader; }

private:
    Shader m_shader;
    Texture m_whiteTexture;
    Mesh m_quad;
    glm::mat4 m_viewProjection{1.0f};
};
