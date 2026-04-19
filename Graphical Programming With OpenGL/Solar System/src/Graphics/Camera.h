#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

enum class CameraMovement
{
    Forward,
    Backward,
    Left,
    Right
};

class Camera
{
public:
    explicit Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f);

    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xOffset, float yOffset);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;
    const glm::vec3& GetPosition() const;

private:
    void UpdateVectors();

    glm::vec3 m_position;
    glm::vec3 m_worldUp;
    glm::vec3 m_front{0.0f, 0.0f, -1.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};
    float m_yaw;
    float m_pitch;
    float m_movementSpeed{18.0f};
    float m_mouseSensitivity{0.1f};
    float m_zoom{45.0f};
};