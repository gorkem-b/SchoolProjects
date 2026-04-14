#include "Camera.h"

#include <GL/glut.h>

#include <cmath>

namespace
{
    const float PI = 3.14159265358979323846f;
}

Camera::Camera()
    : m_yaw(45.0f)
    , m_pitch(-20.0f)
    , m_distance(14.0f)
    , m_targetX(0.0f)
    , m_targetY(2.3f)
    , m_targetZ(0.0f)
    , m_dragging(false)
    , m_lastMouseX(0)
    , m_lastMouseY(0)
    , m_viewportWidth(1280)
    , m_viewportHeight(720)
{
}

void Camera::applyProjection() const
{
    const double aspectRatio = (m_viewportHeight != 0)
        ? static_cast<double>(m_viewportWidth) / static_cast<double>(m_viewportHeight)
        : 1.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspectRatio, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Camera::applyView() const
{
    const float yawRadians = m_yaw * PI / 180.0f;
    const float pitchRadians = m_pitch * PI / 180.0f;

    const float cameraX = m_targetX + m_distance * std::cos(pitchRadians) * std::sin(yawRadians);
    const float cameraY = m_targetY + m_distance * std::sin(pitchRadians);
    const float cameraZ = m_targetZ + m_distance * std::cos(pitchRadians) * std::cos(yawRadians);

    gluLookAt(cameraX, cameraY, cameraZ,
              m_targetX, m_targetY, m_targetZ,
              0.0f, 1.0f, 0.0f);
}

void Camera::setViewport(int width, int height)
{
    m_viewportWidth = (width > 1) ? width : 1;
    m_viewportHeight = (height > 1) ? height : 1;
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
}

void Camera::onMouseButton(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON)
    {
        m_dragging = (state == GLUT_DOWN);
        m_lastMouseX = x;
        m_lastMouseY = y;
        return;
    }

    if(state == GLUT_DOWN && button == 3)
    {
        zoom(-0.6f);
        return;
    }

    if(state == GLUT_DOWN && button == 4)
    {
        zoom(0.6f);
        return;
    }
}

void Camera::onMouseMove(int x, int y)
{
    if(!m_dragging)
    {
        return;
    }

    const float sensitivity = 0.35f;
    const int dx = x - m_lastMouseX;
    const int dy = y - m_lastMouseY;

    m_yaw += static_cast<float>(dx) * sensitivity;
    m_pitch -= static_cast<float>(dy) * sensitivity;
    m_pitch = clamp(m_pitch, -80.0f, 80.0f);

    m_lastMouseX = x;
    m_lastMouseY = y;
}

void Camera::zoom(float delta)
{
    m_distance = clamp(m_distance + delta, 4.0f, 30.0f);
}

float Camera::clamp(float value, float minValue, float maxValue)
{
    if(value < minValue)
    {
        return minValue;
    }

    if(value > maxValue)
    {
        return maxValue;
    }

    return value;
}