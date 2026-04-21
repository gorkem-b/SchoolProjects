#pragma once

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
    explicit Camera(float aspectRatio);

    void SetAspectRatio(float aspectRatio);
    void Translate(const glm::vec3& delta);
    void UpdateFollowingPlane(const glm::vec3& planePosition, const glm::vec3& planeVelocity, const glm::quat& planeOrientation, float deltaTime);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    const glm::vec3& GetPosition() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_aspectRatio;
    float m_fieldOfViewDegrees = 60.0f;
};