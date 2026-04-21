#pragma once

#include <array>
#include <string>

#include <glm/mat4x4.hpp>

class Shader;

class Skybox
{
public:
    Skybox() = default;
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    bool Load(const std::array<std::string, 6>& facePaths);
    void Draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection) const;

private:
    void Release();

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_cubemapTexture = 0;
};