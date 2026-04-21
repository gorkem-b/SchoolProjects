#pragma once

#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Plane
{
public:
    Plane();

    void SetSpawnState(const glm::vec3& position, const glm::quat& orientation, float speed, float throttle);
    void SetPosition(const glm::vec3& position);
    void Reset();
    void Update(float deltaTime, bool allowInput = true);
    void ConstrainAboveGround(float minimumAltitude);

    glm::mat4 GetModelMatrix() const;
    const glm::vec3& GetPosition() const;
    const glm::quat& GetOrientation() const;
    const glm::vec3& GetVelocity() const;
    float GetCurrentSpeed() const;
    float GetThrottle() const;
    float GetAngleOfAttackDegrees() const;
    float GetGForce() const;
    float GetIndicatedAirspeedKmh() const;
    float GetPitchDegrees() const;
    float GetBankDegrees() const;
    float GetFlightPathMarkerX() const;
    float GetFlightPathMarkerY() const;
    float GetTrim() const;

private:
    glm::vec3 GetForward() const;
    glm::vec3 GetUp() const;
    glm::vec3 GetRight() const;

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::quat m_orientation;
    float m_throttle;
    float m_currentSpeed;
    float m_aileronInput;
    float m_elevatorInput;
    float m_rudderInput;
    float m_trim;
    float m_angleOfAttackDegrees;
    glm::vec3 m_spawnPosition;
    glm::quat m_spawnOrientation;
    float m_spawnSpeed;
    float m_spawnThrottle;
};