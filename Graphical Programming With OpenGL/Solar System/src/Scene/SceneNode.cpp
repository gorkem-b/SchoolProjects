#include "Scene/SceneNode.h"

#include "Graphics/Renderer.h"

#include <glm/geometric.hpp>

namespace
{
glm::mat4 RemoveScaleFromTransform(const glm::mat4& transform)
{
    glm::mat4 result = transform;

    const auto normalizeAxis = [&result, &transform](int columnIndex) {
        const glm::vec3 axis = glm::vec3(transform[columnIndex]);
        const float axisLength = glm::length(axis);
        if (axisLength > 0.0001f)
        {
            result[columnIndex] = glm::vec4(axis / axisLength, 0.0f);
        }
    };

    normalizeAxis(0);
    normalizeAxis(1);
    normalizeAxis(2);
    result[3] = glm::vec4(glm::vec3(transform[3]), 1.0f);

    return result;
}
}

void SceneNode::AddChild(const std::shared_ptr<SceneNode>& child)
{
    m_children.push_back(child);
}

void SceneNode::Update(float deltaTime)
{
    OnUpdate(deltaTime);

    for (const auto& child : m_children)
    {
        child->Update(deltaTime);
    }
}

void SceneNode::UpdateWorldTransform(const glm::mat4& parentWorldTransform)
{
    const glm::mat4 inheritedParentTransform = InheritsParentScale()
        ? parentWorldTransform
        : RemoveScaleFromTransform(parentWorldTransform);

    m_worldTransform = inheritedParentTransform * m_localTransform;

    for (const auto& child : m_children)
    {
        child->UpdateWorldTransform(m_worldTransform);
    }
}

void SceneNode::Draw(Renderer& renderer, const RenderContext& context) const
{
    OnDraw(renderer, context);

    for (const auto& child : m_children)
    {
        child->Draw(renderer, context);
    }
}

const glm::mat4& SceneNode::GetLocalTransform() const
{
    return m_localTransform;
}

const glm::mat4& SceneNode::GetWorldTransform() const
{
    return m_worldTransform;
}

void SceneNode::SetLocalTransform(const glm::mat4& transform)
{
    m_localTransform = transform;
}

void SceneNode::OnUpdate(float)
{
}

void SceneNode::OnDraw(Renderer&, const RenderContext&) const
{
}

bool SceneNode::InheritsParentScale() const
{
    return true;
}