#include "Renderer/Shader.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>
#include <utility>
#include <vector>

namespace {
std::string shaderLog(unsigned int shader)
{
    int length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> log(static_cast<std::size_t>(length > 0 ? length : 1), '\0');
    glGetShaderInfoLog(shader, static_cast<int>(log.size()), nullptr, log.data());
    return log.data();
}

std::string programLog(unsigned int program)
{
    int length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> log(static_cast<std::size_t>(length > 0 ? length : 1), '\0');
    glGetProgramInfoLog(program, static_cast<int>(log.size()), nullptr, log.data());
    return log.data();
}

unsigned int compileShader(unsigned int type, const char* source)
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
        const std::string log = shaderLog(shader);
        glDeleteShader(shader);
        throw std::runtime_error("Failed to compile OpenGL shader: " + log);
    }
    return shader;
}
}

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
    const unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragment = 0;
    try {
        fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    } catch (const std::runtime_error&) {
        glDeleteShader(vertex);
        throw;
    }

    program_ = glCreateProgram();
    if (!program_) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        throw std::runtime_error("Failed to create OpenGL program");
    }

    glAttachShader(program_, vertex);
    glAttachShader(program_, fragment);
    glLinkProgram(program_);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    int success = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        const std::string log = programLog(program_);
        glDeleteProgram(program_);
        throw std::runtime_error("Failed to link OpenGL program: " + log);
    }
}

Shader::~Shader()
{
    glDeleteProgram(program_);
}

Shader::Shader(Shader&& other) noexcept
    : program_(std::exchange(other.program_, 0)),
      locations_(std::move(other.locations_))
{
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other) {
        glDeleteProgram(program_);
        program_ = std::exchange(other.program_, 0);
        locations_ = std::move(other.locations_);
    }
    return *this;
}

void Shader::bind() const
{
    glUseProgram(program_);
}

int Shader::location(const std::string& name) const
{
    const auto found = locations_.find(name);
    if (found != locations_.end()) {
        return found->second;
    }
    const int result = glGetUniformLocation(program_, name.c_str());
    locations_.emplace(name, result);
    return result;
}

void Shader::setInt(const std::string& name, int value) const
{
    bind();
    glUniform1i(location(name), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    bind();
    glUniform1f(location(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    bind();
    glUniform2fv(location(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    bind();
    glUniform3fv(location(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    bind();
    glUniform4fv(location(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    bind();
    glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}
