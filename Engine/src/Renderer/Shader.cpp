#include "Renderer/Shader.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>
#include <utility>
#include <vector>

namespace {
std::string ShaderLog(unsigned int shader)
{
    int length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> log(static_cast<std::size_t>(length > 0 ? length : 1), '\0');
    glGetShaderInfoLog(shader, static_cast<int>(log.size()), nullptr, log.data());
    return log.data();
}

std::string ProgramLog(unsigned int program)
{
    int length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> log(static_cast<std::size_t>(length > 0 ? length : 1), '\0');
    glGetProgramInfoLog(program, static_cast<int>(log.size()), nullptr, log.data());
    return log.data();
}

unsigned int CompileShader(unsigned int type, const char* source)
{
    const unsigned int shader = glCreateShader(type);
    if (!shader) {
        throw std::runtime_error("Failed to create OpenGL shader");
    }

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        const std::string log = ShaderLog(shader);
        glDeleteShader(shader);
        throw std::runtime_error("Failed to compile OpenGL shader: " + log);
    }
    return shader;
}
}

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
    const unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragment = 0;
    try {
        fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    } catch (const std::runtime_error&) {
        glDeleteShader(vertex);
        throw;
    }

    m_program = glCreateProgram();
    if (!m_program) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        throw std::runtime_error("Failed to create OpenGL program");
    }

    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        const std::string log = ProgramLog(m_program);
        glDeleteProgram(m_program);
        throw std::runtime_error("Failed to link OpenGL program: " + log);
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_program);
}

Shader::Shader(Shader&& other) noexcept
    : m_program(std::exchange(other.m_program, 0)),
      m_locations(std::move(other.m_locations))
{
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other) {
        glDeleteProgram(m_program);
        m_program = std::exchange(other.m_program, 0);
        m_locations = std::move(other.m_locations);
    }
    return *this;
}

void Shader::Bind() const
{
    glUseProgram(m_program);
}

int Shader::Location(const std::string& name) const
{
    const auto found = m_locations.find(name);
    if (found != m_locations.end()) {
        return found->second;
    }
    const int result = glGetUniformLocation(m_program, name.c_str());
    m_locations.emplace(name, result);
    return result;
}

void Shader::SetInt(const std::string& name, int value) const
{
    Bind();
    glUniform1i(Location(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    Bind();
    glUniform1f(Location(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    Bind();
    glUniform2fv(Location(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    Bind();
    glUniform3fv(Location(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    Bind();
    glUniform4fv(Location(name), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    Bind();
    glUniformMatrix4fv(Location(name), 1, GL_FALSE, glm::value_ptr(value));
}
