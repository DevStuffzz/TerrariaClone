#pragma once

#include "Core/Base.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <unordered_map>

class ENGINE_API Shader {
public:
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void Bind() const;

    // Uniform setters bind the shader. Unknown uniform names are ignored,
    // matching OpenGL's behavior for location -1.
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

    unsigned int Id() const { return m_program; }

private:
    int Location(const std::string& name) const;

    unsigned int m_program = 0;
    mutable std::unordered_map<std::string, int> m_locations;
};
