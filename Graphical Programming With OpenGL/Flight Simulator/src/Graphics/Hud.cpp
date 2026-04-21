#include "Graphics/Hud.h"

#include <array>

#include <glad/gl.h>

#include "Graphics/Shader.h"

namespace {
struct HudVertex
{
    float x;
    float y;
};
}

Hud::~Hud()
{
    Release();
}

bool Hud::Initialize()
{
    Release();

    constexpr std::array<HudVertex, 4> vertices = {
        HudVertex{-1.0f, -1.0f},
        HudVertex{ 1.0f, -1.0f},
        HudVertex{ 1.0f,  1.0f},
        HudVertex{-1.0f,  1.0f},
    };

    constexpr std::array<unsigned int, 6> indices = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HudVertex), nullptr);

    glBindVertexArray(0);
    return true;
}

void Hud::Draw(const Shader& shader, int viewportWidth, int viewportHeight, const HudData& data) const
{
    if (m_vao == 0)
    {
        return;
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.Use();
    shader.SetFloat("uAspectRatio", viewportHeight == 0 ? 1.0f : static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));
    shader.SetFloat("uAltitude", data.altitudeMeters);
    shader.SetFloat("uSpeed", data.speedKmh);
    shader.SetFloat("uIAS", data.indicatedAirspeedKmh);
    shader.SetFloat("uThrottle", data.throttlePercent / 100.0f);
    shader.SetFloat("uAoA", data.angleOfAttackDegrees);
    shader.SetFloat("uGForce", data.gForce);
    shader.SetFloat("uPitch", data.pitchDegrees);
    shader.SetFloat("uBank", data.bankDegrees);
    shader.SetFloat("uFpmX", data.flightPathMarkerX);
    shader.SetFloat("uFpmY", data.flightPathMarkerY);
    shader.SetFloat("uTrim", data.trimValue);
    shader.SetFloat("uPaused", data.paused ? 1.0f : 0.0f);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Hud::Release()
{
    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }

    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}