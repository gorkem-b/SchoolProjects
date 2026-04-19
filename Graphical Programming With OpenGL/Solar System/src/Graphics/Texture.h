#pragma once

#include <array>
#include <glad/gl.h>

#include <filesystem>
#include <memory>
#include <string>

struct ColorRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class Texture
{
public:
    static std::shared_ptr<Texture> Load2D(const std::filesystem::path& path, ColorRGB fallbackColor);
    static std::shared_ptr<Texture> LoadCubemap(const std::array<std::filesystem::path, 6>& faces, const std::array<ColorRGB, 6>& fallbackColors);

    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void Bind(unsigned int textureUnit = 0) const;
    GLenum GetTarget() const;

private:
    explicit Texture(GLenum target);
    static void UploadFallback2D(ColorRGB fallbackColor);

    GLuint m_id{0};
    GLenum m_target;
};