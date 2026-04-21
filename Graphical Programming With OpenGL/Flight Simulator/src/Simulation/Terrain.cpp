#include "Simulation/Terrain.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <glm/geometric.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"
#include <SOIL/SOIL.h>

namespace {
float Fract(float value)
{
    return value - std::floor(value);
}

float Hash(float x, float z)
{
    return Fract(std::sin(x * 127.1f + z * 311.7f) * 43758.5453f);
}

float SmoothStep(float value)
{
    return value * value * (3.0f - 2.0f * value);
}

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
}

bool Terrain::InitializeProcedural(int resolution, float spacing, const Texture* texture, const Texture* normalTexture)
{
    const int columns = resolution + 1;
    const int rows = resolution + 1;
    std::vector<float> heights(static_cast<std::size_t>(columns * rows));

    const float halfExtentX = (columns - 1) * spacing * 0.5f;
    const float halfExtentZ = (rows - 1) * spacing * 0.5f;

    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            const float x = column * spacing - halfExtentX;
            const float z = row * spacing - halfExtentZ;
            heights[static_cast<std::size_t>(row * columns + column)] = SampleHeight(x, z);
        }
    }

    m_texture = texture;
    m_normalTexture = normalTexture;
    return BuildTerrainMesh(heights, columns, rows, spacing);
}

bool Terrain::InitializeFromHeightmap(const std::string& heightmapPath, float heightScale, float spacing, const Texture* texture,
                                      const Texture* normalTexture)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* imageData = SOIL_load_image(heightmapPath.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
    if (imageData == nullptr)
    {
        std::cerr << "Failed to load heightmap '" << heightmapPath << "': " << SOIL_last_result() << std::endl;
        return false;
    }

    const int imageChannels = channels <= 0 ? 3 : channels;
    std::vector<float> heights(static_cast<std::size_t>(width * height));

    for (int row = 0; row < height; ++row)
    {
        for (int column = 0; column < width; ++column)
        {
            const std::size_t pixelIndex = static_cast<std::size_t>((row * width + column) * imageChannels);
            const float red = static_cast<float>(imageData[pixelIndex + 0]);
            const float green = static_cast<float>(imageData[pixelIndex + std::min(1, imageChannels - 1)]);
            const float blue = static_cast<float>(imageData[pixelIndex + std::min(2, imageChannels - 1)]);
            const float luminance = (red + green + blue) / (3.0f * 255.0f);
            heights[static_cast<std::size_t>(row * width + column)] = (luminance - 0.5f) * heightScale;
        }
    }

    SOIL_free_image_data(imageData);
    m_texture = texture;
    m_normalTexture = normalTexture;
    return BuildTerrainMesh(heights, width, height, spacing);
}

void Terrain::Draw(const Shader& shader, const TextureManager& textureManager) const
{
    shader.SetMat4("model", m_model);
    shader.SetInt("diffuseTexture", 0);
    shader.SetInt("normalTexture", 1);
    textureManager.Bind2D(m_texture, 0);
    textureManager.Bind2D(m_normalTexture != nullptr ? m_normalTexture : m_texture, 1);
    m_mesh.Draw();
}

bool Terrain::BuildTerrainMesh(const std::vector<float>& heights, int columns, int rows, float spacing)
{
    if (columns < 2 || rows < 2 || heights.size() != static_cast<std::size_t>(columns * rows))
    {
        return false;
    }

    m_heightSamples = heights;
    m_gridColumns = columns;
    m_gridRows = rows;
    m_spacing = spacing;
    m_halfExtentX = (columns - 1) * spacing * 0.5f;
    m_halfExtentZ = (rows - 1) * spacing * 0.5f;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(static_cast<std::size_t>(columns * rows));
    indices.reserve(static_cast<std::size_t>((columns - 1) * (rows - 1) * 6));

    const float textureTiling = 24.0f;

    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            const float x = column * spacing - m_halfExtentX;
            const float z = row * spacing - m_halfExtentZ;
            const float y = m_heightSamples[static_cast<std::size_t>(row * columns + column)];

            const int leftColumn = std::max(column - 1, 0);
            const int rightColumn = std::min(column + 1, columns - 1);
            const int downRow = std::max(row - 1, 0);
            const int upRow = std::min(row + 1, rows - 1);

            const float leftHeight = m_heightSamples[static_cast<std::size_t>(row * columns + leftColumn)];
            const float rightHeight = m_heightSamples[static_cast<std::size_t>(row * columns + rightColumn)];
            const float downHeight = m_heightSamples[static_cast<std::size_t>(downRow * columns + column)];
            const float upHeight = m_heightSamples[static_cast<std::size_t>(upRow * columns + column)];

            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::normalize(glm::vec3(leftHeight - rightHeight, 2.0f * spacing, downHeight - upHeight));
            vertex.texCoord = glm::vec2(
                static_cast<float>(column) / static_cast<float>(columns - 1) * textureTiling,
                static_cast<float>(row) / static_cast<float>(rows - 1) * textureTiling
            );

            vertices.push_back(vertex);
        }
    }

    for (int row = 0; row < rows - 1; ++row)
    {
        for (int column = 0; column < columns - 1; ++column)
        {
            const unsigned int topLeft = static_cast<unsigned int>(row * columns + column);
            const unsigned int topRight = topLeft + 1;
            const unsigned int bottomLeft = topLeft + static_cast<unsigned int>(columns);
            const unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_mesh.Upload(vertices, indices);
    return m_mesh.IsValid();
}

float Terrain::GetHeightAt(float x, float z) const
{
    if (m_heightSamples.empty() || m_gridColumns < 2 || m_gridRows < 2)
    {
        return 0.0f;
    }

    const float gridX = std::clamp((x + m_halfExtentX) / m_spacing, 0.0f, static_cast<float>(m_gridColumns - 1));
    const float gridZ = std::clamp((z + m_halfExtentZ) / m_spacing, 0.0f, static_cast<float>(m_gridRows - 1));

    const int x0 = static_cast<int>(std::floor(gridX));
    const int z0 = static_cast<int>(std::floor(gridZ));
    const int x1 = std::min(x0 + 1, m_gridColumns - 1);
    const int z1 = std::min(z0 + 1, m_gridRows - 1);

    const float tx = gridX - static_cast<float>(x0);
    const float tz = gridZ - static_cast<float>(z0);

    const float h00 = m_heightSamples[static_cast<std::size_t>(z0 * m_gridColumns + x0)];
    const float h10 = m_heightSamples[static_cast<std::size_t>(z0 * m_gridColumns + x1)];
    const float h01 = m_heightSamples[static_cast<std::size_t>(z1 * m_gridColumns + x0)];
    const float h11 = m_heightSamples[static_cast<std::size_t>(z1 * m_gridColumns + x1)];

    const float hx0 = Lerp(h00, h10, tx);
    const float hx1 = Lerp(h01, h11, tx);
    return Lerp(hx0, hx1, tz);
}

bool Terrain::IsWithinBounds(float x, float z, float margin) const
{
    return x >= -m_halfExtentX - margin &&
           x <= m_halfExtentX + margin &&
           z >= -m_halfExtentZ - margin &&
           z <= m_halfExtentZ + margin;
}

float Terrain::GetHalfExtentX() const
{
    return m_halfExtentX;
}

float Terrain::GetHalfExtentZ() const
{
    return m_halfExtentZ;
}

float Terrain::SampleHeight(float x, float z) const
{
    float amplitude = 18.0f;
    float frequency = 0.018f;
    float total = 0.0f;
    float normalization = 0.0f;

    for (int octave = 0; octave < 5; ++octave)
    {
        total += ValueNoise(x * frequency, z * frequency) * amplitude;
        normalization += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.1f;
    }

    total /= normalization;
    total = total * 42.0f;
    total += std::sin(x * 0.02f) * 2.5f;
    total += std::cos(z * 0.018f) * 2.0f;
    return total;
}

float Terrain::ValueNoise(float x, float z) const
{
    const float x0 = std::floor(x);
    const float z0 = std::floor(z);
    const float x1 = x0 + 1.0f;
    const float z1 = z0 + 1.0f;

    const float tx = SmoothStep(Fract(x));
    const float tz = SmoothStep(Fract(z));

    const float v00 = Hash(x0, z0);
    const float v10 = Hash(x1, z0);
    const float v01 = Hash(x0, z1);
    const float v11 = Hash(x1, z1);

    const float nx0 = Lerp(v00, v10, tx);
    const float nx1 = Lerp(v01, v11, tx);
    return Lerp(nx0, nx1, tz) * 2.0f - 1.0f;
}