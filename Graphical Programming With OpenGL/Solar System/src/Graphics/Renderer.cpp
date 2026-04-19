#include "Graphics/Renderer.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Scene/CelestialBody.h"

#include <glad/gl.h>

#include <stdexcept>

void Renderer::SetShaders(Shader* litShader, Shader* unlitShader, Shader* skyboxShader)
{
    m_litShader = litShader;
    m_unlitShader = unlitShader;
    m_skyboxShader = skyboxShader;
}

void Renderer::DrawCelestialBody(const CelestialBody& body, const RenderContext& context) const
{
    Shader* shader = m_litShader;
    if (shader == nullptr)
    {
        throw std::runtime_error("Renderer shaders are not initialized.");
    }

    const Texture* specularMap = body.GetSpecularMap();
    const Texture* emissionMap = body.GetEmissionMap();
    const Texture* cloudMap = body.GetCloudMap();
    const Texture* opacityMap = body.GetOpacityMap();

    const GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);
    const GLboolean wasCullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    GLboolean previousDepthMask = GL_TRUE;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);

    if (body.IsTransparent())
    {
        glEnable(GL_BLEND);
        if (body.UsesAdditiveBlending())
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
        else
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        glDepthMask(GL_FALSE);
    }

    if (body.IsDoubleSided())
    {
        glDisable(GL_CULL_FACE);
    }

    shader->Use();
    shader->SetMat4("model", body.GetWorldTransform());
    shader->SetMat4("view", context.view);
    shader->SetMat4("projection", context.projection);
    shader->SetInt("uTexture", 0);
    shader->SetInt("uSpecularMap", 1);
    shader->SetInt("uEmissionMap", 2);
    shader->SetInt("uCloudMap", 3);
    shader->SetInt("uOpacityMap", 4);
    shader->SetBool("uHasSpecularMap", specularMap != nullptr);
    shader->SetBool("uHasEmissionMap", emissionMap != nullptr);
    shader->SetBool("uHasCloudMap", cloudMap != nullptr);
    shader->SetBool("uHasOpacityMap", opacityMap != nullptr);
    shader->SetBool("uIsEmissive", body.IsEmissive());
    shader->SetVec3("lightPos", context.lightPosition);
    shader->SetVec3("viewPos", context.cameraPosition);
    shader->SetVec3("uTint", body.GetTint());
    shader->SetVec3("uAtmosphereColor", body.GetAtmosphereColor());
    shader->SetFloat("uAmbientStrength", body.GetAmbientStrength());
    shader->SetFloat("uDiffuseStrength", body.GetDiffuseStrength());
    shader->SetFloat("uSpecularStrength", body.GetSpecularStrength());
    shader->SetFloat("uShininess", body.GetShininess());
    shader->SetFloat("uEmissionStrength", body.GetEmissionStrength());
    shader->SetFloat("uAtmosphereStrength", body.GetAtmosphereStrength());
    shader->SetFloat("uCloudOpacity", body.GetCloudOpacity());
    shader->SetFloat("uCloudSpeed", body.GetCloudSpeed());
    shader->SetFloat("uAlpha", body.GetAlpha());
    shader->SetFloat("uTime", context.time);

    body.GetTexture().Bind(0);

    if (specularMap != nullptr)
    {
        specularMap->Bind(1);
    }

    if (emissionMap != nullptr)
    {
        emissionMap->Bind(2);
    }

    if (cloudMap != nullptr)
    {
        cloudMap->Bind(3);
    }

    if (opacityMap != nullptr)
    {
        opacityMap->Bind(4);
    }

    body.GetMesh().Draw();

    glDepthMask(previousDepthMask);

    if (body.IsDoubleSided())
    {
        if (wasCullFaceEnabled == GL_TRUE)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }

    if (body.IsTransparent())
    {
        if (wasBlendEnabled == GL_TRUE)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Renderer::DrawSkybox(const Mesh& mesh, const Texture& cubemap, const glm::mat4& view, const glm::mat4& projection) const
{
    if (m_skyboxShader == nullptr)
    {
        throw std::runtime_error("Skybox shader is not initialized.");
    }

    const GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    m_skyboxShader->Use();
    m_skyboxShader->SetMat4("view", view);
    m_skyboxShader->SetMat4("projection", projection);
    m_skyboxShader->SetInt("skybox", 0);

    cubemap.Bind(0);
    mesh.Draw();

    if (cullFaceEnabled == GL_TRUE)
    {
        glEnable(GL_CULL_FACE);
    }

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}