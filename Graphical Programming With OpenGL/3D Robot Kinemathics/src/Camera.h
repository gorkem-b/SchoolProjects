#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
    Camera();

    void applyProjection() const;
    void applyView() const;
    void setViewport(int width, int height);

    void onMouseButton(int button, int state, int x, int y);
    void onMouseMove(int x, int y);
    void zoom(float delta);

private:
    static float clamp(float value, float minValue, float maxValue);

    float m_yaw;
    float m_pitch;
    float m_distance;

    float m_targetX;
    float m_targetY;
    float m_targetZ;

    bool m_dragging;
    int m_lastMouseX;
    int m_lastMouseY;
    int m_viewportWidth;
    int m_viewportHeight;
};

#endif