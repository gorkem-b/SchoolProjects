#pragma once

#include <string>

#include "Graphics/Mesh.h"

class Shader;
class Texture;
class TextureManager;

class Model
{
public:
    bool Load(const std::string& modelPath, const Texture* texture);
    void Draw(const Shader& shader, const TextureManager& textureManager) const;
    bool IsLoaded() const;

private:
    Mesh m_mesh;
    const Texture* m_texture = nullptr;
    bool m_loaded = false;
};