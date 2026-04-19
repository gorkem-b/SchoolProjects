#pragma once

#include "Scene/SceneNode.h"

#include <glm/vec3.hpp>

#include <memory>
#include <string>

class Mesh;
class Texture;

struct CelestialBodyConfig
{
    std::string name;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Texture> specularMap;
    std::shared_ptr<Texture> emissionMap;
    std::shared_ptr<Texture> cloudMap;
    std::shared_ptr<Texture> opacityMap;
    float radius{1.0f};
    float orbitRadius{0.0f};
    float orbitSpeed{0.0f};
    float rotationSpeed{0.0f};
    float orbitInclination{0.0f};
    float axialTilt{0.0f};
    float initialOrbitAngle{0.0f};
    glm::vec3 tint{1.0f, 1.0f, 1.0f};
    glm::vec3 atmosphereColor{0.0f, 0.0f, 0.0f};
    float ambientStrength{0.16f};
    float diffuseStrength{0.95f};
    float specularStrength{0.18f};
    float shininess{32.0f};
    float emissionStrength{0.0f};
    float atmosphereStrength{0.0f};
    float cloudOpacity{0.0f};
    float cloudSpeed{0.0f};
    float alpha{1.0f};
    bool emissive{false};
    bool transparent{false};
    bool doubleSided{false};
    bool additiveBlending{false};
    bool inheritParentScale{false};
};

class CelestialBody : public SceneNode
{
public:
    explicit CelestialBody(CelestialBodyConfig config);

    const std::string& GetName() const;
    const Mesh& GetMesh() const;
    const Texture& GetTexture() const;
    const Texture* GetSpecularMap() const;
    const Texture* GetEmissionMap() const;
    const Texture* GetCloudMap() const;
    const Texture* GetOpacityMap() const;
    const glm::vec3& GetTint() const;
    const glm::vec3& GetAtmosphereColor() const;
    float GetAmbientStrength() const;
    float GetDiffuseStrength() const;
    float GetSpecularStrength() const;
    float GetShininess() const;
    float GetEmissionStrength() const;
    float GetAtmosphereStrength() const;
    float GetCloudOpacity() const;
    float GetCloudSpeed() const;
    float GetAlpha() const;
    bool IsEmissive() const;
    bool IsTransparent() const;
    bool IsDoubleSided() const;
    bool UsesAdditiveBlending() const;

private:
    void OnUpdate(float deltaTime) override;
    void OnDraw(Renderer& renderer, const RenderContext& context) const override;
    bool InheritsParentScale() const override;

    CelestialBodyConfig m_config;
    float m_orbitAngle{0.0f};
    float m_rotationAngle{0.0f};
};