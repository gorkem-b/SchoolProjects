#pragma once

#include <glm/vec3.hpp>

#include <memory>

class Renderer;
struct RenderContext;
class SceneNode;

class Scene
{
public:
    void SetRoot(const std::shared_ptr<SceneNode>& root);
    void Update(float deltaTime);
    void Draw(Renderer& renderer, const RenderContext& context) const;
    glm::vec3 GetRootWorldPosition() const;

private:
    std::shared_ptr<SceneNode> m_root;
};