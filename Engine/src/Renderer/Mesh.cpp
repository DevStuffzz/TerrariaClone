#include "Renderer/Mesh.h"

#include <glad/gl.h>

#include <cstddef>
#include <stdexcept>
#include <utility>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices,
           MeshUsage usage)
    : m_usage(usage)
{
    glGenVertexArrays(1, &m_vertexArray);
    glGenBuffers(1, &m_vertexBuffer);
    glGenBuffers(1, &m_indexBuffer);
    if (!m_vertexArray || !m_vertexBuffer || !m_indexBuffer) {
        Release();
        throw std::runtime_error("Failed to create mesh buffers");
    }

    glBindVertexArray(m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    // The element buffer binding is stored in the VAO, so bind it while the VAO is bound.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

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
        SetData(vertices, indices);
    } catch (...) {
        Release();
        throw;
    }
}

Mesh::~Mesh()
{
    Release();
}

void Mesh::Release()
{
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteVertexArrays(1, &m_vertexArray);
    m_indexBuffer = m_vertexBuffer = m_vertexArray = 0;
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vertexArray(std::exchange(other.m_vertexArray, 0)),
      m_vertexBuffer(std::exchange(other.m_vertexBuffer, 0)),
      m_indexBuffer(std::exchange(other.m_indexBuffer, 0)),
      m_vertexCount(std::exchange(other.m_vertexCount, 0)),
      m_indexCount(std::exchange(other.m_indexCount, 0)),
      m_usage(other.m_usage)
{
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other) {
        Release();
        m_vertexArray = std::exchange(other.m_vertexArray, 0);
        m_vertexBuffer = std::exchange(other.m_vertexBuffer, 0);
        m_indexBuffer = std::exchange(other.m_indexBuffer, 0);
        m_vertexCount = std::exchange(other.m_vertexCount, 0);
        m_indexCount = std::exchange(other.m_indexCount, 0);
        m_usage = other.m_usage;
    }
    return *this;
}

Mesh Mesh::CreateQuad(const glm::vec4& color)
{
    const std::vector<Vertex> vertices = {
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, color},
        {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, color},
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, color},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, color},
    };
    return Mesh(vertices, {0, 1, 2, 2, 3, 0});
}

void Mesh::SetData(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
{
    for (const std::uint32_t index : indices) {
        if (index >= vertices.size()) {
            throw std::out_of_range("Mesh index refers to a missing vertex");
        }
    }

    const GLenum glUsage = m_usage == MeshUsage::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
    glBindVertexArray(m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), glUsage);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)),
                 indices.data(), glUsage);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vertexCount = vertices.size();
    m_indexCount = indices.size();
}

void Mesh::Draw() const
{
    if (m_vertexCount == 0) {
        return;
    }
    glBindVertexArray(m_vertexArray);
    if (m_indexCount > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
    }
    glBindVertexArray(0);
}
