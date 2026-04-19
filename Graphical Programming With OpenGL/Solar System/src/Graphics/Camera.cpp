#include "Graphics/Camera.h"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
    : m_position(position)
    , m_worldUp(worldUp)
    , m_yaw(yaw)
    , m_pitch(pitch)
{
    UpdateVectors();
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    const float velocity = m_movementSpeed * deltaTime;

    switch (direction)
    {
    case CameraMovement::Forward:
        m_position += m_front * velocity;
        break;
    case CameraMovement::Backward:
        m_position -= m_front * velocity;
        break;
    case CameraMovement::Left:
        m_position -= m_right * velocity;
        break;
    case CameraMovement::Right:
        m_position += m_right * velocity;
        break;
    }
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(m_zoom), aspectRatio, 0.1f, 500.0f);
}

const glm::vec3& Camera::GetPosition() const
{
    return m_position;
}

void Camera::UpdateVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}