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
    Dynamic  // updated often via setData (e.g. rebuilt tile chunks)
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
    static Mesh createQuad(const glm::vec4& color = glm::vec4(1.0f));

    void setData(const std::vector<Vertex>& vertices,
                 const std::vector<std::uint32_t>& indices = {});
    void draw() const;

    std::size_t vertexCount() const { return vertexCount_; }
    std::size_t indexCount() const { return indexCount_; }

private:
    void release();

    unsigned int vertexArray_ = 0;
    unsigned int vertexBuffer_ = 0;
    unsigned int indexBuffer_ = 0;
    std::size_t vertexCount_ = 0;
    std::size_t indexCount_ = 0;
    MeshUsage usage_ = MeshUsage::Static;
};
