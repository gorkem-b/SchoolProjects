#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Graphics/Texture.h"

class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    const Texture* Load2D(const std::string& path, bool srgb = true);
    void Bind2D(const Texture* texture, unsigned int slot = 0) const;

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
    mutable unsigned int m_lastBoundTexture = 0;
    mutable unsigned int m_lastActiveSlot = 0;
};