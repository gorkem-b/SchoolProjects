#include "Graphics/Model.h"

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/geometric.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"

namespace {
struct IndexTriplet
{
    int position = 0;
    int texCoord = 0;
    int normal = 0;
};

IndexTriplet ParseVertexToken(const std::string& token)
{
    IndexTriplet result;

    std::stringstream stream(token);
    std::string segment;
    std::vector<std::string> segments;

    while (std::getline(stream, segment, '/'))
    {
        segments.push_back(segment);
    }

    if (!segments.empty() && !segments[0].empty())
    {
        result.position = std::stoi(segments[0]);
    }

    if (segments.size() > 1 && !segments[1].empty())
    {
        result.texCoord = std::stoi(segments[1]);
    }

    if (segments.size() > 2 && !segments[2].empty())
    {
        result.normal = std::stoi(segments[2]);
    }

    return result;
}

int ResolveIndex(int rawIndex, int count)
{
    if (rawIndex > 0)
    {
        return rawIndex - 1;
    }

    return count + rawIndex;
}
}

bool Model::Load(const std::string& modelPath, const Texture* texture)
{
    std::ifstream file(modelPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open model file: " << modelPath << std::endl;
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<std::string, unsigned int> uniqueVertices;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::stringstream stream(line);
        std::string prefix;
        stream >> prefix;

        if (prefix == "v")
        {
            glm::vec3 position;
            stream >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (prefix == "vt")
        {
            glm::vec2 texCoord;
            stream >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        else if (prefix == "vn")
        {
            glm::vec3 normal;
            stream >> normal.x >> normal.y >> normal.z;
            normals.push_back(glm::normalize(normal));
        }
        else if (prefix == "f")
        {
            std::vector<std::string> faceTokens;
            std::string faceToken;
            while (stream >> faceToken)
            {
                faceTokens.push_back(faceToken);
            }

            if (faceTokens.size() < 3)
            {
                continue;
            }

            for (std::size_t triangleVertex = 1; triangleVertex + 1 < faceTokens.size(); ++triangleVertex)
            {
                const std::array<std::string, 3> triangle = {
                    faceTokens[0],
                    faceTokens[triangleVertex],
                    faceTokens[triangleVertex + 1],
                };

                for (const std::string& token : triangle)
                {
                    const auto found = uniqueVertices.find(token);
                    if (found != uniqueVertices.end())
                    {
                        indices.push_back(found->second);
                        continue;
                    }

                    const IndexTriplet indexTriplet = ParseVertexToken(token);
                    Vertex vertex;

                    if (indexTriplet.position != 0)
                    {
                        vertex.position = positions[ResolveIndex(indexTriplet.position, static_cast<int>(positions.size()))];
                    }

                    if (indexTriplet.texCoord != 0)
                    {
                        vertex.texCoord = texCoords[ResolveIndex(indexTriplet.texCoord, static_cast<int>(texCoords.size()))];
                    }

                    if (indexTriplet.normal != 0)
                    {
                        vertex.normal = normals[ResolveIndex(indexTriplet.normal, static_cast<int>(normals.size()))];
                    }

                    const unsigned int nextIndex = static_cast<unsigned int>(vertices.size());
                    uniqueVertices[token] = nextIndex;
                    vertices.push_back(vertex);
                    indices.push_back(nextIndex);
                }
            }
        }
    }

    if (normals.empty())
    {
        std::vector<glm::vec3> accumulatedNormals(vertices.size(), glm::vec3(0.0f));

        for (std::size_t index = 0; index + 2 < indices.size(); index += 3)
        {
            const unsigned int i0 = indices[index + 0];
            const unsigned int i1 = indices[index + 1];
            const unsigned int i2 = indices[index + 2];

            const glm::vec3 edgeOne = vertices[i1].position - vertices[i0].position;
            const glm::vec3 edgeTwo = vertices[i2].position - vertices[i0].position;
            const glm::vec3 faceNormal = glm::normalize(glm::cross(edgeOne, edgeTwo));

            accumulatedNormals[i0] += faceNormal;
            accumulatedNormals[i1] += faceNormal;
            accumulatedNormals[i2] += faceNormal;
        }

        for (std::size_t vertexIndex = 0; vertexIndex < vertices.size(); ++vertexIndex)
        {
            if (glm::length(accumulatedNormals[vertexIndex]) > 0.0f)
            {
                vertices[vertexIndex].normal = glm::normalize(accumulatedNormals[vertexIndex]);
            }
        }
    }

    m_mesh.Upload(vertices, indices);
    m_texture = texture;
    m_loaded = m_mesh.IsValid();
    return m_loaded;
}

void Model::Draw(const Shader& shader, const TextureManager& textureManager) const
{
    if (!m_loaded)
    {
        return;
    }

    shader.SetInt("diffuseTexture", 0);
    textureManager.Bind2D(m_texture, 0);
    m_mesh.Draw();
}

bool Model::IsLoaded() const
{
    return m_loaded;
}