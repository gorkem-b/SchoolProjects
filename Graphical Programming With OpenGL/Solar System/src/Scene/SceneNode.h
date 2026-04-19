#pragma once

#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

class Renderer;
struct RenderContext;

class SceneNode
{
public:
    virtual ~SceneNode() = default;

    void AddChild(const std::shared_ptr<SceneNode>& child);
    void Update(float deltaTime);
    void UpdateWorldTransform(const glm::mat4& parentWorldTransform = glm::mat4(1.0f));
    void Draw(Renderer& renderer, const RenderContext& context) const;

    const glm::mat4& GetLocalTransform() const;
    const glm::mat4& GetWorldTransform() const;

protected:
    void SetLocalTransform(const glm::mat4& transform);
    virtual void OnUpdate(float deltaTime);
    virtual void OnDraw(Renderer& renderer, const RenderContext& context) const;
    virtual bool InheritsParentScale() const;

private:
    std::vector<std::shared_ptr<SceneNode>> m_children;
    glm::mat4 m_localTransform{1.0f};
    glm::mat4 m_worldTransform{1.0f};
};