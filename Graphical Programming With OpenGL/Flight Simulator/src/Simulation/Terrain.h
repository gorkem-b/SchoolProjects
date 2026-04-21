#pragma once

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "Graphics/Mesh.h"

class Shader;
class Texture;
class TextureManager;

class Terrain
{
public:
    Terrain() = default;

    bool InitializeProcedural(int resolution, float spacing, const Texture* texture, const Texture* normalTexture = nullptr);
    bool InitializeFromHeightmap(const std::string& heightmapPath, float heightScale, float spacing, const Texture* texture,
                                 const Texture* normalTexture = nullptr);
    void Draw(const Shader& shader, const TextureManager& textureManager) const;
    float GetHeightAt(float x, float z) const;
    bool IsWithinBounds(float x, float z, float margin = 0.0f) const;
    float GetHalfExtentX() const;
    float GetHalfExtentZ() const;

private:
    bool BuildTerrainMesh(const std::vector<float>& heights, int columns, int rows, float spacing);
    float SampleHeight(float x, float z) const;
    float ValueNoise(float x, float z) const;

    Mesh m_mesh;
    const Texture* m_texture = nullptr;
    const Texture* m_normalTexture = nullptr;
    glm::mat4 m_model = glm::mat4(1.0f);
    std::vector<float> m_heightSamples;
    int m_gridColumns = 0;
    int m_gridRows = 0;
    float m_spacing = 1.0f;
    float m_halfExtentX = 0.0f;
    float m_halfExtentZ = 0.0f;
};