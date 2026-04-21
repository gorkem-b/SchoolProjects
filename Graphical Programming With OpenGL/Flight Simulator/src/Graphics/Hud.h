#pragma once

#include <string>

class Shader;

struct HudData
{
    float altitudeMeters = 0.0f;
    float speedKmh = 0.0f;
    float indicatedAirspeedKmh = 0.0f;
    float throttlePercent = 0.0f;
    float angleOfAttackDegrees = 0.0f;
    float gForce = 1.0f;
    float pitchDegrees = 0.0f;
    float bankDegrees = 0.0f;
    float flightPathMarkerX = 0.0f;
    float flightPathMarkerY = 0.0f;
    float trimValue = 0.0f;
    bool paused = false;
};

class Hud
{
public:
    Hud() = default;
    ~Hud();

    Hud(const Hud&) = delete;
    Hud& operator=(const Hud&) = delete;

    bool Initialize();
    void Draw(const Shader& shader, int viewportWidth, int viewportHeight, const HudData& data) const;

private:
    void Release();

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
};