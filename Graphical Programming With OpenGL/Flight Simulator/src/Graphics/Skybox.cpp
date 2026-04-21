#include "Graphics/Skybox.h"

#include <array>
#include <iostream>

#include <glad/gl.h>

#include <glm/mat3x3.hpp>

#include <SOIL/SOIL.h>

#include "Graphics/Shader.h"

Skybox::~Skybox()
{
    Release();
}

bool Skybox::Load(const std::array<std::string, 6>& facePaths)
{
    Release();

    constexpr std::array<float, 108> skyboxVertices = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindVertexArray(0);

    glGenTextures(1, &m_cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);

    constexpr std::array<std::array<unsigned char, 3>, 6> fallbackColors = {
        std::array<unsigned char, 3>{ 135, 206, 235 },
        std::array<unsigned char, 3>{ 120, 170, 220 },
        std::array<unsigned char, 3>{ 255, 200, 140 },
        std::array<unsigned char, 3>{ 40, 60, 90 },
        std::array<unsigned char, 3>{ 140, 190, 230 },
        std::array<unsigned char, 3>{ 140, 190, 230 },
    };

    bool allFacesLoaded = true;
    for (std::size_t faceIndex = 0; faceIndex < facePaths.size(); ++faceIndex)
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* data = SOIL_load_image(facePaths[faceIndex].c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);

        if (data == nullptr)
        {
            allFacesLoaded = false;
            const auto& color = fallbackColors[faceIndex];
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(faceIndex),
                0,
                GL_RGB,
                1,
                1,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                color.data()
            );
            continue;
        }

        const GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        const GLenum internalFormat = channels == 4 ? GL_SRGB_ALPHA : GL_SRGB;
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(faceIndex),
            0,
            static_cast<GLint>(internalFormat),
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );
        SOIL_free_image_data(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if (!allFacesLoaded)
    {
        std::cerr << "One or more skybox faces failed to load. Using fallback cubemap colors." << std::endl;
    }

    return true;
}

void Skybox::Draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection) const
{
    if (m_vao == 0 || m_cubemapTexture == 0)
    {
        return;
    }

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    shader.Use();
    shader.SetMat4("view", glm::mat4(glm::mat3(view)));
    shader.SetMat4("projection", projection);
    shader.SetInt("skybox", 0);

    glBindVertexArray(m_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void Skybox::Release()
{
    if (m_cubemapTexture != 0)
    {
        glDeleteTextures(1, &m_cubemapTexture);
        m_cubemapTexture = 0;
    }

    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}