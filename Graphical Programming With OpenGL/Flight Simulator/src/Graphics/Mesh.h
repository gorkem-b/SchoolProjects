#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec2 texCoord = glm::vec2(0.0f);
};

class Mesh
{
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Draw() const;
    bool IsValid() const;

private:
    void Release();

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
    int m_indexCount = 0;
};