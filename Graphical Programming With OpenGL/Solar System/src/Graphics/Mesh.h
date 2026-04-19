#pragma once

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Draw() const;

    static std::shared_ptr<Mesh> CreateSphere(unsigned int sectorCount = 64, unsigned int stackCount = 64);
    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreateRing(float innerRadius = 0.45f, float outerRadius = 1.0f, unsigned int segmentCount = 256);

private:
    void Setup();

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    GLuint m_vao{0};
    GLuint m_vbo{0};
    GLuint m_ebo{0};
};