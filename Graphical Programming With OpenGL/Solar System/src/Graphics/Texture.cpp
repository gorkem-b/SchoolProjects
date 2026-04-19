#include "Graphics/Texture.h"

#include <stb_image.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <vector>

namespace
{
std::vector<unsigned char> ReadBinaryFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return {};
    }

    const std::streamsize size = file.tellg();
    if (size <= 0)
    {
        return {};
    }

    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(static_cast<std::size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        return {};
    }

    return buffer;
}

unsigned char* LoadImageFromFile(const std::filesystem::path& path, int* width, int* height, int* channels)
{
    const auto fileBytes = ReadBinaryFile(path);
    if (fileBytes.empty())
    {
        return nullptr;
    }

    return stbi_load_from_memory(
        fileBytes.data(),
        static_cast<int>(fileBytes.size()),
        width,
        height,
        channels,
        0);
}
}

Texture::Texture(GLenum target)
    : m_target(target)
{
    glGenTextures(1, &m_id);
}

Texture::~Texture()
{
    if (m_id != 0)
    {
        glDeleteTextures(1, &m_id);
    }
}

std::shared_ptr<Texture> Texture::Load2D(const std::filesystem::path& path, ColorRGB fallbackColor)
{
    auto texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_2D));

    glBindTexture(GL_TEXTURE_2D, texture->m_id);

    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = LoadImageFromFile(path, &width, &height, &channels);

    if (data != nullptr)
    {
        GLenum format = GL_RGB;
        if (channels == 4)
        {
            format = GL_RGBA;
        }
        else if (channels == 1)
        {
            format = GL_RED;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        UploadFallback2D(fallbackColor);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

std::shared_ptr<Texture> Texture::LoadCubemap(const std::array<std::filesystem::path, 6>& faces, const std::array<ColorRGB, 6>& fallbackColors)
{
    auto texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_CUBE_MAP));

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_id);

    stbi_set_flip_vertically_on_load(0);

    for (std::size_t index = 0; index < faces.size(); ++index)
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* data = LoadImageFromFile(faces[index], &width, &height, &channels);

        if (data != nullptr)
        {
            GLenum format = GL_RGB;
            if (channels == 4)
            {
                format = GL_RGBA;
            }
            else if (channels == 1)
            {
                format = GL_RED;
            }

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(index),
                0,
                static_cast<GLint>(format),
                width,
                height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data);

            stbi_image_free(data);
        }
        else
        {
            const unsigned char pixel[3] {
                fallbackColors[index].r,
                fallbackColors[index].g,
                fallbackColors[index].b
            };

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(index),
                0,
                GL_RGB,
                1,
                1,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                pixel);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}

void Texture::Bind(unsigned int textureUnit) const
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(m_target, m_id);
}

GLenum Texture::GetTarget() const
{
    return m_target;
}

void Texture::UploadFallback2D(ColorRGB fallbackColor)
{
    const unsigned char pixel[3] { fallbackColor.r, fallbackColor.g, fallbackColor.b };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    glGenerateMipmap(GL_TEXTURE_2D);
}