#include "Scene/CelestialBody.h"

#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <utility>

CelestialBody::CelestialBody(CelestialBodyConfig config)
    : m_config(std::move(config))
    , m_orbitAngle(m_config.initialOrbitAngle)
{
    if (!m_config.mesh)
    {
        throw std::runtime_error("Celestial body mesh is missing.");
    }

    if (!m_config.texture)
    {
        throw std::runtime_error("Celestial body texture is missing.");
    }
}

const std::string& CelestialBody::GetName() const
{
    return m_config.name;
}

const Mesh& CelestialBody::GetMesh() const
{
    return *m_config.mesh;
}

const Texture& CelestialBody::GetTexture() const
{
    return *m_config.texture;
}

const Texture* CelestialBody::GetSpecularMap() const
{
    return m_config.specularMap.get();
}

const Texture* CelestialBody::GetEmissionMap() const
{
    return m_config.emissionMap.get();
}

const Texture* CelestialBody::GetCloudMap() const
{
    return m_config.cloudMap.get();
}

const Texture* CelestialBody::GetOpacityMap() const
{
    return m_config.opacityMap.get();
}

const glm::vec3& CelestialBody::GetTint() const
{
    return m_config.tint;
}

const glm::vec3& CelestialBody::GetAtmosphereColor() const
{
    return m_config.atmosphereColor;
}

float CelestialBody::GetAmbientStrength() const
{
    return m_config.ambientStrength;
}

float CelestialBody::GetDiffuseStrength() const
{
    return m_config.diffuseStrength;
}

float CelestialBody::GetSpecularStrength() const
{
    return m_config.specularStrength;
}

float CelestialBody::GetShininess() const
{
    return m_config.shininess;
}

float CelestialBody::GetEmissionStrength() const
{
    return m_config.emissionStrength;
}

float CelestialBody::GetAtmosphereStrength() const
{
    return m_config.atmosphereStrength;
}

float CelestialBody::GetCloudOpacity() const
{
    return m_config.cloudOpacity;
}

float CelestialBody::GetCloudSpeed() const
{
    return m_config.cloudSpeed;
}

float CelestialBody::GetAlpha() const
{
    return m_config.alpha;
}

bool CelestialBody::IsEmissive() const
{
    return m_config.emissive;
}

bool CelestialBody::IsTransparent() const
{
    return m_config.transparent;
}

bool CelestialBody::IsDoubleSided() const
{
    return m_config.doubleSided;
}

bool CelestialBody::UsesAdditiveBlending() const
{
    return m_config.additiveBlending;
}

void CelestialBody::OnUpdate(float deltaTime)
{
    m_orbitAngle += m_config.orbitSpeed * deltaTime;
    m_rotationAngle += m_config.rotationSpeed * deltaTime;

    glm::mat4 transform(1.0f);
    transform = glm::rotate(transform, glm::radians(m_orbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(m_config.orbitInclination), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(m_config.orbitRadius, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(m_config.axialTilt), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::rotate(transform, glm::radians(m_rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::scale(transform, glm::vec3(m_config.radius));

    SetLocalTransform(transform);
}

void CelestialBody::OnDraw(Renderer& renderer, const RenderContext& context) const
{
    renderer.DrawCelestialBody(*this, context);
}

bool CelestialBody::InheritsParentScale() const
{
    return m_config.inheritParentScale;
}