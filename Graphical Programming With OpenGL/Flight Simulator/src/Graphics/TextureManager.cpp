#include "Graphics/TextureManager.h"

#include <glad/gl.h>

#include "Graphics/Texture.h"

TextureManager::~TextureManager() = default;

const Texture* TextureManager::Load2D(const std::string& path, bool srgb)
{
    const std::string cacheKey = path + (srgb ? "#srgb" : "#linear");
    const auto found = m_textures.find(cacheKey);
    if (found != m_textures.end())
    {
        return found->second.get();
    }

    auto texture = std::make_unique<Texture>();
    texture->LoadFromFile(path, srgb);

    const Texture* texturePtr = texture.get();
    m_textures.emplace(cacheKey, std::move(texture));
    return texturePtr;
}

void TextureManager::Bind2D(const Texture* texture, unsigned int slot) const
{
    if (texture == nullptr)
    {
        return;
    }

    if (m_lastActiveSlot != slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        m_lastActiveSlot = slot;
        m_lastBoundTexture = 0;
    }

    const unsigned int textureId = texture->GetId();
    if (m_lastBoundTexture != textureId)
    {
        glBindTexture(GL_TEXTURE_2D, textureId);
        m_lastBoundTexture = textureId;
    }
}