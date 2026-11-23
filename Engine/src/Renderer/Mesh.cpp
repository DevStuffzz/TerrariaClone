#include "Renderer/Mesh.h"

#include <glad/gl.h>

#include <cstddef>
#include <stdexcept>
#include <utility>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices,
           MeshUsage usage)
    : usage_(usage)
{
    glGenVertexArrays(1, &vertexArray_);
    glGenBuffers(1, &vertexBuffer_);
    glGenBuffers(1, &indexBuffer_);
    if (!vertexArray_ || !vertexBuffer_ || !indexBuffer_) {
        release();
        throw std::runtime_error("Failed to create mesh buffers");
    }

    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    // The element buffer binding is stored in the VAO, so bind it while the VAO is bound.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);

    const auto stride = static_cast<GLsizei>(sizeof(Vertex));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    try {
        setData(vertices, indices);
    } catch (...) {
        release();
        throw;
    }
}

Mesh::~Mesh()
{
    release();
}

void Mesh::release()
{
    glDeleteBuffers(1, &indexBuffer_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteVertexArrays(1, &vertexArray_);
    indexBuffer_ = vertexBuffer_ = vertexArray_ = 0;
}

Mesh::Mesh(Mesh&& other) noexcept
    : vertexArray_(std::exchange(other.vertexArray_, 0)),
      vertexBuffer_(std::exchange(other.vertexBuffer_, 0)),
      indexBuffer_(std::exchange(other.indexBuffer_, 0)),
      vertexCount_(std::exchange(other.vertexCount_, 0)),
      indexCount_(std::exchange(other.indexCount_, 0)),
      usage_(other.usage_)
{
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other) {
        release();
        vertexArray_ = std::exchange(other.vertexArray_, 0);
        vertexBuffer_ = std::exchange(other.vertexBuffer_, 0);
        indexBuffer_ = std::exchange(other.indexBuffer_, 0);
        vertexCount_ = std::exchange(other.vertexCount_, 0);
        indexCount_ = std::exchange(other.indexCount_, 0);
        usage_ = other.usage_;
    }
    return *this;
}

Mesh Mesh::createQuad(const glm::vec4& color)
{
    const std::vector<Vertex> vertices = {
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, color},
        {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, color},
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, color},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, color},
    };
    return Mesh(vertices, {0, 1, 2, 2, 3, 0});
}

void Mesh::setData(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
{
    for (const std::uint32_t index : indices) {
        if (index >= vertices.size()) {
            throw std::out_of_range("Mesh index refers to a missing vertex");
        }
    }

    const GLenum glUsage = usage_ == MeshUsage::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), glUsage);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)),
                 indices.data(), glUsage);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vertexCount_ = vertices.size();
    indexCount_ = indices.size();
}

void Mesh::draw() const
{
    if (vertexCount_ == 0) {
        return;
    }
    glBindVertexArray(vertexArray_);
    if (indexCount_ > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount_), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount_));
    }
    glBindVertexArray(0);
}
