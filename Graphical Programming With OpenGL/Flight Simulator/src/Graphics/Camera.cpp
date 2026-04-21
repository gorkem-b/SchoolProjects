#include "Graphics/Camera.h"

#include <algorithm>
#include <cmath>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Config/Tuning.h"

Camera::Camera(float aspectRatio)
    : m_position(0.0f, 20.0f, 30.0f)
    , m_target(0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_aspectRatio(aspectRatio)
    , m_fieldOfViewDegrees(Config::Camera.fov)
{
}

void Camera::SetAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

void Camera::Translate(const glm::vec3& delta)
{
    m_position += delta;
    m_target += delta;
}

void Camera::UpdateFollowingPlane(const glm::vec3& planePosition, const glm::vec3& planeVelocity, const glm::quat& planeOrientation, float deltaTime)
{
    (void)planeVelocity;

    const auto& tuning = Config::Camera;
    const glm::vec3 forward = planeOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraForward(forward.x, 0.0f, forward.z);
    if (glm::dot(cameraForward, cameraForward) < 0.0001f)
    {
        cameraForward = glm::vec3(m_target.x - m_position.x, 0.0f, m_target.z - m_position.z);
    }

    if (glm::dot(cameraForward, cameraForward) < 0.0001f)
    {
        cameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
    }
    else
    {
        cameraForward = glm::normalize(cameraForward);
    }

    glm::vec3 desiredPosition = planePosition - cameraForward * tuning.distance + worldUp * tuning.height;
    glm::vec3 desiredTarget = planePosition + cameraForward * tuning.lookAhead + worldUp * tuning.targetHeight;

    desiredPosition.y = glm::mix(m_position.y, planePosition.y + tuning.height, tuning.verticalFollowStrength);
    desiredTarget.x = planePosition.x + cameraForward.x * tuning.lookAhead;
    desiredTarget.z = planePosition.z + cameraForward.z * tuning.lookAhead;
    desiredTarget.y = glm::mix(m_target.y, planePosition.y + tuning.targetHeight, tuning.verticalFollowStrength);

    const float blend = deltaTime <= 0.0f ? 1.0f : std::clamp(1.0f - std::exp(-tuning.blendRate * deltaTime), 0.0f, 1.0f);

    m_position = glm::mix(m_position, desiredPosition, blend);
    m_target = glm::mix(m_target, desiredTarget, blend);
    m_up = worldUp;
    m_fieldOfViewDegrees = glm::mix(m_fieldOfViewDegrees, tuning.fov, std::clamp(deltaTime * 4.0f, 0.0f, 1.0f));
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(m_fieldOfViewDegrees), m_aspectRatio, 0.1f, Config::Camera.farPlane);
}

const glm::vec3& Camera::GetPosition() const
{
    return m_position;
}