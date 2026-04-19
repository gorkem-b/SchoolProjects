#include "Scene/Scene.h"

#include "Graphics/Renderer.h"
#include "Scene/SceneNode.h"

void Scene::SetRoot(const std::shared_ptr<SceneNode>& root)
{
    m_root = root;
}

void Scene::Update(float deltaTime)
{
    if (!m_root)
    {
        return;
    }

    m_root->Update(deltaTime);
    m_root->UpdateWorldTransform();
}

void Scene::Draw(Renderer& renderer, const RenderContext& context) const
{
    if (!m_root)
    {
        return;
    }

    m_root->Draw(renderer, context);
}

glm::vec3 Scene::GetRootWorldPosition() const
{
    if (!m_root)
    {
        return glm::vec3(0.0f);
    }

    return glm::vec3(m_root->GetWorldTransform()[3]);
}