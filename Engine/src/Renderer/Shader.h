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

    void bind() const;

    // Uniform setters bind the shader. Unknown uniform names are ignored,
    // matching OpenGL's behavior for location -1.
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

    unsigned int id() const { return program_; }

private:
    int location(const std::string& name) const;

    unsigned int program_ = 0;
    mutable std::unordered_map<std::string, int> locations_;
};
