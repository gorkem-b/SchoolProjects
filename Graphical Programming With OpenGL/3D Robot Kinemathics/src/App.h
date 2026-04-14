#ifndef APP_H
#define APP_H

#include "Camera.h"
#include "Robot.h"

#include <string>

class App
{
public:
    App();

    int run(int argc, char** argv);

    void render();
    void reshape(int width, int height);
    void idle();
    void requestRedisplay() const;
    void toggleLighting();
    void toggleWireframe();
    void togglePause();
    bool isLightingEnabled() const;
    bool isWireframeEnabled() const;
    bool isPaused() const;

    Robot& getRobot();
    Camera& getCamera();
    const Robot& getRobot() const;
    const Camera& getCamera() const;

private:
    void initialize();
    void configureOpenGL();
    void applyLighting() const;
    void drawFloor(float halfExtent) const;
    void drawGrid(int halfExtent, float spacing) const;
    void drawAxes(float length) const;
    void drawOverlay() const;
    void drawText2D(int x, int y, const std::string& text) const;

    static void displayCallback();
    static void reshapeCallback(int width, int height);
    static void idleCallback();

    static App* s_instance;

    int m_windowWidth;
    int m_windowHeight;
    int m_lastElapsedTimeMs;
    float m_fixedDeltaTime;
    float m_accumulator;
    bool m_lightingEnabled;
    bool m_wireframeEnabled;
    bool m_paused;
    Robot m_robot;
    Camera m_camera;
};

#endif