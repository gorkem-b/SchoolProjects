#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class CelestialBody;
class Mesh;
class Shader;
class Texture;

struct RenderContext
{
    glm::mat4 view{1.0f};
    glm::mat4 projection{1.0f};
    glm::vec3 cameraPosition{0.0f};
    glm::vec3 lightPosition{0.0f};
    float time{0.0f};
};

class Renderer
{
public:
    void SetShaders(Shader* litShader, Shader* unlitShader, Shader* skyboxShader);
    void DrawCelestialBody(const CelestialBody& body, const RenderContext& context) const;
    void DrawSkybox(const Mesh& mesh, const Texture& cubemap, const glm::mat4& view, const glm::mat4& projection) const;

private:
    Shader* m_litShader{nullptr};
    Shader* m_unlitShader{nullptr};
    Shader* m_skyboxShader{nullptr};
};