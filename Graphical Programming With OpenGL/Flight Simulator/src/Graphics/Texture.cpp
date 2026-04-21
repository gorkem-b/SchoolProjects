#include "Graphics/Texture.h"

#include <algorithm>
#include <array>
#include <iostream>

#include <glad/gl.h>

#include <SOIL/SOIL.h>

namespace {
void GetTextureFormats(int channels, bool srgb, GLenum& internalFormat, GLenum& dataFormat)
{
    if (channels == 4)
    {
        internalFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;
        dataFormat = GL_RGBA;
        return;
    }

    internalFormat = srgb ? GL_SRGB : GL_RGB;
    dataFormat = GL_RGB;
}

void ApplyTextureSamplingParameters()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (GLAD_GL_ARB_texture_filter_anisotropic || GLAD_GL_EXT_texture_filter_anisotropic)
    {
        float maxAnisotropy = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, std::min(16.0f, maxAnisotropy));
    }
}
}

Texture::~Texture()
{
    Release();
}

bool Texture::LoadFromFile(const std::string& path, bool srgb)
{
    Release();
    m_isSrgb = srgb;

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* imageData = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    if (imageData == nullptr)
    {
        std::cerr << "Failed to load texture from '" << path << "': " << SOIL_last_result() << std::endl;
        CreateFallbackCheckerboard();
        return false;
    }

    GLenum internalFormat = GL_RGB;
    GLenum dataFormat = GL_RGB;
    GetTextureFormats(channels, srgb, internalFormat, dataFormat);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width, height, 0, dataFormat, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    ApplyTextureSamplingParameters();

    SOIL_free_image_data(imageData);
    m_loadedFromFile = true;
    return true;
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

unsigned int Texture::GetId() const
{
    return m_textureId;
}

bool Texture::IsLoadedFromFile() const
{
    return m_loadedFromFile;
}

void Texture::CreateFallbackCheckerboard()
{
    constexpr std::array<unsigned char, 16> fallbackPixels = {
        60, 180, 75, 255,
        34, 139, 34, 255,
        34, 139, 34, 255,
        60, 180, 75, 255,
    };

    const GLenum internalFormat = m_isSrgb ? GL_SRGB_ALPHA : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallbackPixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    ApplyTextureSamplingParameters();

    m_loadedFromFile = false;
    m_isSrgb = false;
}

void Texture::Release()
{
    if (m_textureId != 0)
    {
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }

    m_loadedFromFile = false;
}