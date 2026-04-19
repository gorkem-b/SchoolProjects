#include "Graphics/Mesh.h"

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <cstddef>
#include <utility>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : m_vertices(std::move(vertices))
    , m_indices(std::move(indices))
{
    Setup();
}

Mesh::~Mesh()
{
    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
    }

    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

void Mesh::Draw() const
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

std::shared_ptr<Mesh> Mesh::CreateSphere(unsigned int sectorCount, unsigned int stackCount)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve((sectorCount + 1) * (stackCount + 1));

    for (unsigned int stack = 0; stack <= stackCount; ++stack)
    {
        const float stackAngle = glm::pi<float>() / 2.0f - static_cast<float>(stack) * glm::pi<float>() / static_cast<float>(stackCount);
        const float xy = std::cos(stackAngle);
        const float z = std::sin(stackAngle);

        for (unsigned int sector = 0; sector <= sectorCount; ++sector)
        {
            const float sectorAngle = static_cast<float>(sector) * glm::two_pi<float>() / static_cast<float>(sectorCount);

            Vertex vertex;
            vertex.position = glm::vec3(xy * std::cos(sectorAngle), z, xy * std::sin(sectorAngle));
            vertex.normal = glm::normalize(vertex.position);
            vertex.texCoord = glm::vec2(
                static_cast<float>(sector) / static_cast<float>(sectorCount),
                static_cast<float>(stack) / static_cast<float>(stackCount));

            vertices.push_back(vertex);
        }
    }

    for (unsigned int stack = 0; stack < stackCount; ++stack)
    {
        unsigned int k1 = stack * (sectorCount + 1);
        unsigned int k2 = k1 + sectorCount + 1;

        for (unsigned int sector = 0; sector < sectorCount; ++sector, ++k1, ++k2)
        {
            if (stack != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (stack != stackCount - 1)
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return std::make_shared<Mesh>(std::move(vertices), std::move(indices));
}

std::shared_ptr<Mesh> Mesh::CreateCube()
{
    static const glm::vec3 positions[] {
        {-1.0f,  1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f},
        {-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}
    };

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(36);
    indices.reserve(36);

    for (unsigned int index = 0; index < 36; ++index)
    {
        Vertex vertex;
        vertex.position = positions[index];
        vertex.normal = glm::vec3(0.0f);
        vertex.texCoord = glm::vec2(0.0f);

        vertices.push_back(vertex);
        indices.push_back(index);
    }

    return std::make_shared<Mesh>(std::move(vertices), std::move(indices));
}

std::shared_ptr<Mesh> Mesh::CreateRing(float innerRadius, float outerRadius, unsigned int segmentCount)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve((segmentCount + 1) * 2);
    indices.reserve(segmentCount * 6);

    for (unsigned int segment = 0; segment <= segmentCount; ++segment)
    {
        const float t = static_cast<float>(segment) / static_cast<float>(segmentCount);
        const float angle = t * glm::two_pi<float>();
        const float cosAngle = std::cos(angle);
        const float sinAngle = std::sin(angle);

        Vertex innerVertex;
        innerVertex.position = glm::vec3(innerRadius * cosAngle, 0.0f, innerRadius * sinAngle);
        innerVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        innerVertex.texCoord = glm::vec2(t, 0.0f);
        vertices.push_back(innerVertex);

        Vertex outerVertex;
        outerVertex.position = glm::vec3(outerRadius * cosAngle, 0.0f, outerRadius * sinAngle);
        outerVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        outerVertex.texCoord = glm::vec2(t, 1.0f);
        vertices.push_back(outerVertex);
    }

    for (unsigned int segment = 0; segment < segmentCount; ++segment)
    {
        const unsigned int start = segment * 2;

        indices.push_back(start);
        indices.push_back(start + 1);
        indices.push_back(start + 2);

        indices.push_back(start + 1);
        indices.push_back(start + 3);
        indices.push_back(start + 2);
    }

    return std::make_shared<Mesh>(std::move(vertices), std::move(indices));
}

void Mesh::Setup()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)),
        m_vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)),
        m_indices.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, texCoord)));

    glBindVertexArray(0);
}