#pragma once

#include <string>

class Texture
{
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    bool LoadFromFile(const std::string& path, bool srgb = true);
    void Bind(unsigned int slot = 0) const;
    unsigned int GetId() const;
    bool IsLoadedFromFile() const;

private:
    void CreateFallbackCheckerboard();
    void Release();

    unsigned int m_textureId = 0;
    bool m_loadedFromFile = false;
    bool m_isSrgb = false;
};