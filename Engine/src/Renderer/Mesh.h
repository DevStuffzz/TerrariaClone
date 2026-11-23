#pragma once

#include "Core/Base.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <vector>

struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec2 texCoord{0.0f};
    glm::vec4 color{1.0f};
};

enum class MeshUsage {
    Static,  // uploaded once, drawn many times
    Dynamic  // updated often via SetData (e.g. rebuilt tile chunks)
};

class ENGINE_API Mesh {
public:
    // Indices are optional; without them vertices are drawn as a triangle list.
    explicit Mesh(const std::vector<Vertex>& vertices,
                  const std::vector<std::uint32_t>& indices = {},
                  MeshUsage usage = MeshUsage::Static);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Unit quad from (0, 0) to (1, 1) with UVs covering the full texture.
    static Mesh CreateQuad(const glm::vec4& color = glm::vec4(1.0f));

    void SetData(const std::vector<Vertex>& vertices,
                 const std::vector<std::uint32_t>& indices = {});
    void Draw() const;

    std::size_t VertexCount() const { return m_vertexCount; }
    std::size_t IndexCount() const { return m_indexCount; }

private:
    void Release();

    unsigned int m_vertexArray = 0;
    unsigned int m_vertexBuffer = 0;
    unsigned int m_indexBuffer = 0;
    std::size_t m_vertexCount = 0;
    std::size_t m_indexCount = 0;
    MeshUsage m_usage = MeshUsage::Static;
};
