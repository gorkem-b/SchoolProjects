#include "Simulation/Plane.h"

#include <algorithm>
#include <cmath>

#include <GLFW/glfw3.h>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Config/Tuning.h"
#include "Core/Input.h"

namespace {
float MoveToward(float current, float target, float rate, float deltaTime)
{
    if (current < target)
    {
        return std::min(current + rate * deltaTime, target);
    }

    return std::max(current - rate * deltaTime, target);
}
}

Plane::Plane()
    : m_position(0.0f)
    , m_velocity(0.0f)
    , m_orientation(1.0f, 0.0f, 0.0f, 0.0f)
    , m_throttle(0.0f)
    , m_currentSpeed(0.0f)
    , m_aileronInput(0.0f)
    , m_elevatorInput(0.0f)
    , m_rudderInput(0.0f)
    , m_trim(0.0f)
    , m_angleOfAttackDegrees(0.0f)
    , m_spawnPosition(0.0f, 180.0f, 250.0f)
    , m_spawnOrientation(1.0f, 0.0f, 0.0f, 0.0f)
    , m_spawnSpeed(58.0f)
    , m_spawnThrottle(0.55f)
{
    Reset();
}

void Plane::SetSpawnState(const glm::vec3& position, const glm::quat& orientation, float speed, float throttle)
{
    m_spawnPosition = position;
    m_spawnOrientation = glm::normalize(orientation);
    m_spawnSpeed = speed;
    m_spawnThrottle = throttle;
}

void Plane::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void Plane::Reset()
{
    m_position = m_spawnPosition;
    m_orientation = m_spawnOrientation;
    m_throttle = m_spawnThrottle;
    m_velocity = GetForward() * m_spawnSpeed;
    m_currentSpeed = glm::length(m_velocity);
    m_aileronInput = 0.0f;
    m_elevatorInput = 0.0f;
    m_rudderInput = 0.0f;
    m_trim = 0.0f;
    m_angleOfAttackDegrees = 0.0f;
}

void Plane::Update(float deltaTime, bool allowInput)
{
    const auto& tuning = Config::PlaneDynamics;
    bool handsOffControls = false;

    if (allowInput)
    {
        handsOffControls =
            !Input::IsKeyDown(GLFW_KEY_W) &&
            !Input::IsKeyDown(GLFW_KEY_S) &&
            !Input::IsKeyDown(GLFW_KEY_A) &&
            !Input::IsKeyDown(GLFW_KEY_D);

        const float elevatorTarget = static_cast<float>(Input::IsKeyDown(GLFW_KEY_S) - Input::IsKeyDown(GLFW_KEY_W));
        const float aileronTarget = static_cast<float>(Input::IsKeyDown(GLFW_KEY_A) - Input::IsKeyDown(GLFW_KEY_D));
        const float rudderTarget = 0.0f;

        m_elevatorInput = MoveToward(m_elevatorInput, elevatorTarget, tuning.elevatorResponseRate, deltaTime);
        m_aileronInput = MoveToward(m_aileronInput, aileronTarget, tuning.aileronResponseRate, deltaTime);
        m_rudderInput = MoveToward(m_rudderInput, rudderTarget, tuning.rudderResponseRate, deltaTime);
    }
    else
    {
        m_elevatorInput = MoveToward(m_elevatorInput, 0.0f, tuning.inputReturnRate, deltaTime);
        m_aileronInput = MoveToward(m_aileronInput, 0.0f, tuning.aileronReturnRate, deltaTime);
        m_rudderInput = MoveToward(m_rudderInput, 0.0f, tuning.inputReturnRate, deltaTime);
    }

    m_trim = 0.0f;
    m_throttle = std::clamp(m_spawnThrottle, 0.0f, 1.0f);

    const glm::vec3 forward = GetForward();
    const glm::vec3 up = GetUp();
    const glm::vec3 right = GetRight();

    const float speed = std::max(glm::length(m_velocity), 0.1f);
    const float controlAuthority = std::clamp(speed / tuning.controlAuthorityReferenceSpeed, tuning.minimumControlAuthority, 1.0f);
    const float bankAmount = glm::dot(right, glm::vec3(0.0f, 1.0f, 0.0f));

    float pitchRate = glm::radians(tuning.pitchRateDegrees) * m_elevatorInput * controlAuthority;
    float rollRate = glm::radians(tuning.rollRateDegrees) * m_aileronInput * controlAuthority;
    float yawRate = glm::radians(tuning.yawRateDegrees) * m_rudderInput * controlAuthority;

    const float aoaPitchStability = glm::radians(m_angleOfAttackDegrees * tuning.pitchStabilityPerAoADegree);
    pitchRate -= aoaPitchStability;

    if (std::abs(m_elevatorInput) < 0.001f)
    {
        const float pitchDegrees = GetPitchDegrees();
        pitchRate += glm::radians(-pitchDegrees * tuning.straightFlightPitchAssistDegrees / 10.0f) * controlAuthority;
    }

    if (std::abs(m_aileronInput) < 0.01f)
    {
        rollRate += -bankAmount * glm::radians(tuning.autoLevelRateDegrees) * std::clamp(speed / tuning.autoLevelReferenceSpeed, 0.0f, 1.0f);
    }

    yawRate += -bankAmount * glm::radians(tuning.bankYawCouplingDegrees) * std::clamp(speed / tuning.bankYawReferenceSpeed, 0.0f, 1.0f);

    if (speed < tuning.stallSpeed)
    {
        const float stallFactor = 1.0f - std::clamp(speed / tuning.stallSpeed, 0.0f, 1.0f);
        pitchRate -= glm::radians(tuning.stallPitchDropDegrees) * stallFactor;
    }

    const glm::quat deltaRotation =
        glm::angleAxis(yawRate * deltaTime, up) *
        glm::angleAxis(pitchRate * deltaTime, right) *
        glm::angleAxis(-rollRate * deltaTime, forward);

    m_orientation = glm::normalize(deltaRotation * m_orientation);

    if (handsOffControls)
    {
        glm::vec3 leveledForward = GetForward();
        leveledForward.y = 0.0f;

        if (glm::dot(leveledForward, leveledForward) > 0.0001f)
        {
            leveledForward = glm::normalize(leveledForward);
            const glm::vec3 desiredRight = glm::normalize(glm::cross(leveledForward, glm::vec3(0.0f, 1.0f, 0.0f)));
            const glm::vec3 desiredUp = glm::normalize(glm::cross(desiredRight, leveledForward));
            const glm::mat3 desiredBasis(desiredRight, desiredUp, -leveledForward);
            const glm::quat desiredOrientation = glm::normalize(glm::quat_cast(desiredBasis));
            const float levelBlend = std::clamp(deltaTime * tuning.straightFlightOrientationAssistRate, 0.0f, 1.0f);
            m_orientation = glm::normalize(glm::slerp(m_orientation, desiredOrientation, levelBlend));
        }
    }

    const glm::vec3 updatedForward = GetForward();
    const glm::vec3 updatedUp = GetUp();
    const float airDensity = 1.225f * std::exp(-std::max(m_position.y, 0.0f) / 9000.0f);
    const float forwardAirspeed = std::max(glm::dot(m_velocity, updatedForward), 0.0f);
    const float dynamicPressure = 0.5f * airDensity * forwardAirspeed * forwardAirspeed * tuning.wingArea;

    float angleOfAttackDegrees = 0.0f;
    if (speed > 0.5f)
    {
        const glm::vec3 flightDirection = glm::normalize(m_velocity);
        angleOfAttackDegrees = glm::degrees(std::asin(glm::clamp(glm::dot(flightDirection, updatedUp), -1.0f, 1.0f)));
    }

    m_angleOfAttackDegrees = angleOfAttackDegrees;

    float liftCoefficient = tuning.liftBaseCoefficient + std::clamp(angleOfAttackDegrees / tuning.liftAoADivisor, -0.35f, 1.15f);
    liftCoefficient += m_trim * tuning.liftControlBias;
    liftCoefficient = std::clamp(liftCoefficient, tuning.liftCoeffMin, tuning.liftCoeffMax);

    if (speed < tuning.stallSpeed)
    {
        liftCoefficient *= std::clamp(speed / tuning.stallSpeed, 0.25f, 1.0f);
    }

    if (std::abs(angleOfAttackDegrees) > tuning.aoaStallBeginDegrees)
    {
        liftCoefficient *= std::clamp(1.0f - (std::abs(angleOfAttackDegrees) - tuning.aoaStallBeginDegrees) / tuning.aoaStallBeginDegrees, 0.25f, 1.0f);
    }

    const glm::vec3 lift = updatedUp * dynamicPressure * liftCoefficient;

    glm::vec3 drag(0.0f);
    if (speed > 0.5f)
    {
        const float dragCoefficient = tuning.baseDragCoefficient + tuning.inducedDragFactor * liftCoefficient * liftCoefficient + (speed < tuning.stallSpeed ? 0.08f : 0.0f);
        drag = -glm::normalize(m_velocity) * dynamicPressure * dragCoefficient;
    }

    const glm::vec3 thrust = updatedForward * (tuning.idleThrust + m_throttle * tuning.maxThrust);
    const glm::vec3 gravity(0.0f, -9.81f * tuning.mass, 0.0f);
    const glm::vec3 acceleration = (thrust + lift + drag + gravity) / tuning.mass;

    m_velocity += acceleration * deltaTime;

    if (handsOffControls)
    {
        glm::vec3 straightForward = GetForward();
        straightForward.y = 0.0f;

        if (glm::dot(straightForward, straightForward) > 0.0001f)
        {
            straightForward = glm::normalize(straightForward);
            const float targetSpeed = std::max(glm::length(m_velocity), m_spawnSpeed);
            m_velocity = straightForward * targetSpeed;
        }
        else
        {
            m_velocity.y = 0.0f;
        }

        m_velocity.y = glm::mix(m_velocity.y, 0.0f, std::clamp(deltaTime * tuning.straightFlightVerticalDamping, 0.0f, 1.0f));
    }

    m_position += m_velocity * deltaTime;
    m_currentSpeed = glm::length(m_velocity);
}

void Plane::ConstrainAboveGround(float minimumAltitude)
{
    if (m_position.y < minimumAltitude)
    {
        m_position.y = minimumAltitude;
        if (m_velocity.y < 0.0f)
        {
            m_velocity.y = 0.0f;
        }
    }
}

glm::mat4 Plane::GetModelMatrix() const
{
    const auto& tuning = Config::PlaneDynamics;
    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position);
    model *= glm::mat4_cast(m_orientation);
    model *= glm::rotate(glm::mat4(1.0f), glm::radians(tuning.modelYawOffsetDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(tuning.modelScale));
    return model;
}

const glm::vec3& Plane::GetPosition() const
{
    return m_position;
}

const glm::quat& Plane::GetOrientation() const
{
    return m_orientation;
}

const glm::vec3& Plane::GetVelocity() const
{
    return m_velocity;
}

float Plane::GetCurrentSpeed() const
{
    return m_currentSpeed;
}

float Plane::GetThrottle() const
{
    return m_throttle;
}

float Plane::GetAngleOfAttackDegrees() const
{
    return m_angleOfAttackDegrees;
}

float Plane::GetGForce() const
{
    const auto& tuning = Config::PlaneDynamics;
    const float speed = glm::length(m_velocity);
    const float bankFactor = std::abs(glm::dot(GetRight(), glm::vec3(0.0f, 1.0f, 0.0f)));
    return 1.0f + bankFactor * std::clamp(speed / tuning.gForceReferenceSpeed, 0.0f, tuning.gForceMaxExtra);
}

float Plane::GetIndicatedAirspeedKmh() const
{
    const auto& tuning = Config::PlaneDynamics;
    const float densityRatio = std::exp(-std::max(m_position.y, 0.0f) / 9000.0f);
    return m_currentSpeed * 3.6f * std::sqrt(std::max(densityRatio, tuning.minimumDensityRatio));
}

float Plane::GetPitchDegrees() const
{
    return glm::degrees(std::asin(glm::clamp(GetForward().y, -1.0f, 1.0f)));
}

float Plane::GetBankDegrees() const
{
    return glm::degrees(std::asin(glm::clamp(GetRight().y, -1.0f, 1.0f)));
}

float Plane::GetFlightPathMarkerX() const
{
    const auto& tuning = Config::PlaneDynamics;
    if (m_currentSpeed <= 0.5f)
    {
        return 0.0f;
    }

    return std::clamp(glm::dot(glm::normalize(m_velocity), GetRight()) * tuning.flightPathMarkerXScale,
                      -tuning.flightPathMarkerClamp, tuning.flightPathMarkerClamp);
}

float Plane::GetFlightPathMarkerY() const
{
    const auto& tuning = Config::PlaneDynamics;
    if (m_currentSpeed <= 0.5f)
    {
        return 0.0f;
    }

    return std::clamp(glm::dot(glm::normalize(m_velocity), GetUp()) * tuning.flightPathMarkerYScale,
                      -tuning.flightPathMarkerClamp, tuning.flightPathMarkerClamp);
}

float Plane::GetTrim() const
{
    return m_trim;
}

glm::vec3 Plane::GetForward() const
{
    return glm::normalize(m_orientation * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Plane::GetUp() const
{
    return glm::normalize(m_orientation * glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Plane::GetRight() const
{
    return glm::normalize(m_orientation * glm::vec3(1.0f, 0.0f, 0.0f));
}