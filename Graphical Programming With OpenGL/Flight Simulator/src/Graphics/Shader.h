#pragma once

#include <string>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Shader
{
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void Use() const;
    bool IsValid() const;

    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

private:
    unsigned int m_program = 0;
};