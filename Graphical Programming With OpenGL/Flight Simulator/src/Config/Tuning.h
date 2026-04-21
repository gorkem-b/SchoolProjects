#pragma once

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace Config {

inline constexpr int kWindowWidth = 1280;
inline constexpr int kWindowHeight = 720;

struct TerrainGenerationSettings
{
    float heightmapHeightScale = 200.0f;
    float heightmapSpacing = 16.0f;
    int proceduralResolution = 300;
    float proceduralSpacing = 14.0f;
    float minimumClearance = 6.0f;
    float wrapMargin = 120.0f;
};

struct SpawnSettings
{
    float zOffset = 1800.0f;
    float altitudeOffset = 260.0f;
    float pitchDegrees = 0.0f;
    float speed = 69.0f;
    float throttle = 0.56f;
};

struct LightingSettings
{
    glm::vec3 direction = glm::normalize(glm::vec3(-0.4f, -1.0f, -0.3f));
    glm::vec3 sunColor = glm::vec3(1.0f, 0.95f, 0.84f);
    glm::vec3 skyAmbientColor = glm::vec3(0.32f, 0.38f, 0.48f);
    glm::vec3 groundAmbientColor = glm::vec3(0.10f, 0.11f, 0.12f);
    glm::vec3 fogColor = glm::vec3(0.55f, 0.75f, 0.95f);
    float fogNearHeightmap = 500.0f;
    float fogFarHeightmap = 6500.0f;
    float fogNearProcedural = 320.0f;
    float fogFarProcedural = 3800.0f;
};

struct PlaneDynamicsSettings
{
    float mass = 1250.0f;
    float wingArea = 18.5f;
    float maxThrust = 14500.0f;
    float idleThrust = 1800.0f;
    float baseDragCoefficient = 0.022f;
    float inducedDragFactor = 0.055f;
    float stallSpeed = 34.0f;
    float throttleChangeRate = 0.32f;

    float elevatorResponseRate = 1.0f;
    float aileronResponseRate = 2.2f;
    float rudderResponseRate = 1.0f;
    float inputReturnRate = 1.2f;
    float aileronReturnRate = 1.8f;
    float trimRate = 0.35f;

    float pitchRateDegrees = 10.0f;
    float rollRateDegrees = 28.0f;
    float yawRateDegrees = 7.0f;
    float autoLevelRateDegrees = 14.0f;
    float bankYawCouplingDegrees = 4.0f;
    float stallPitchDropDegrees = 12.0f;
    float pitchStabilityPerAoADegree = 0.35f;
    float straightFlightPitchAssistDegrees = 4.0f;
    float straightFlightVerticalDamping = 0.85f;
    float straightFlightOrientationAssistRate = 3.0f;

    float controlAuthorityReferenceSpeed = 120.0f;
    float minimumControlAuthority = 0.08f;
    float autoLevelReferenceSpeed = 120.0f;
    float bankYawReferenceSpeed = 110.0f;

    float liftBaseCoefficient = 0.20f;
    float liftAoADivisor = 18.0f;
    float liftControlBias = 0.0f;
    float liftCoeffMin = -0.2f;
    float liftCoeffMax = 1.1f;
    float aoaStallBeginDegrees = 18.0f;

    float gForceReferenceSpeed = 120.0f;
    float gForceMaxExtra = 3.5f;
    float minimumDensityRatio = 0.2f;

    float flightPathMarkerXScale = 0.28f;
    float flightPathMarkerYScale = 0.30f;
    float flightPathMarkerClamp = 0.22f;

    float modelYawOffsetDegrees = 90.0f;
    float modelScale = 0.85f;
};

struct CameraSettings
{
    float distance = 12.0f;
    float height = 4.2f;
    float lookAhead = 2.0f;
    float targetHeight = 1.6f;
    float blendRate = 10.0f;
    float fov = 60.0f;
    float verticalFollowStrength = 0.22f;
    float farPlane = 5000.0f;
};

inline constexpr TerrainGenerationSettings TerrainGeneration{};
inline constexpr SpawnSettings Spawn{};
inline const LightingSettings Lighting{};
inline constexpr PlaneDynamicsSettings PlaneDynamics{};
inline constexpr CameraSettings Camera{};

} // namespace Config