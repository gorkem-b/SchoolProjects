#include "Input.h"

#include "App.h"
#include <GL/glut.h>

#include <cstdlib>
#include <iostream>

namespace
{
    App* g_app = nullptr;
    bool g_shoulderPositive = false;
    bool g_shoulderNegative = false;
    bool g_elbowPositive = false;
    bool g_elbowNegative = false;

    void applyTorqueInput()
    {
        if(g_app == nullptr)
        {
            return;
        }

        const float shoulderAxis = (g_shoulderPositive ? 1.0f : 0.0f) - (g_shoulderNegative ? 1.0f : 0.0f);
        const float elbowAxis = (g_elbowPositive ? 1.0f : 0.0f) - (g_elbowNegative ? 1.0f : 0.0f);
        g_app->getRobot().setControlInput(shoulderAxis, elbowAxis);
    }
}

void Input::bind(App* app)
{
    g_app = app;
}

void Input::registerCallbacks()
{
    glutKeyboardFunc(Input::keyboard);
    glutKeyboardUpFunc(Input::keyboardUp);
    glutMouseFunc(Input::mouseButton);
    glutMotionFunc(Input::mouseMove);
}

void Input::printControls()
{
    std::cout
        << "Controls:\n"
        << "  Q / A : Shoulder torque +/- (hold)\n"
        << "  W / S : Elbow torque +/- (hold)\n"
        << "  1 / 2 / 3 : Free swing / Damped release / Torque control\n"
        << "  R     : Reset current scenario\n"
        << "  Space : Pause / Resume physics\n"
        << "  L     : Toggle lighting\n"
        << "  M     : Toggle wireframe\n"
        << "  B     : Toggle collision debug volumes\n"
        << "  + / - : Zoom in / out\n"
        << "  Mouse drag : Orbit camera\n"
        << "  Mouse wheel: Zoom\n"
        << "  ESC   : Exit\n";
}

void Input::keyboard(unsigned char key, int, int)
{
    if(g_app == nullptr)
    {
        return;
    }

    Robot& robot = g_app->getRobot();
    Camera& camera = g_app->getCamera();
    bool handled = true;

    switch(key)
    {
        case 'q':
        case 'Q':
            g_shoulderPositive = true;
            break;
        case 'a':
        case 'A':
            g_shoulderNegative = true;
            break;
        case 'w':
        case 'W':
            g_elbowPositive = true;
            break;
        case 's':
        case 'S':
            g_elbowNegative = true;
            break;
        case 'r':
        case 'R':
            robot.resetCurrentScenario();
            break;
        case '1':
            robot.setScenario(Robot::ScenarioFreeSwing);
            break;
        case '2':
            robot.setScenario(Robot::ScenarioDampedRelease);
            break;
        case '3':
            robot.setScenario(Robot::ScenarioTorqueControl);
            break;
        case ' ':
            g_app->togglePause();
            break;
        case 'l':
        case 'L':
            g_app->toggleLighting();
            break;
        case 'm':
        case 'M':
            g_app->toggleWireframe();
            break;
        case 'b':
        case 'B':
            robot.toggleDebugDraw();
            break;
        case '+':
        case '=':
            camera.zoom(-0.6f);
            break;
        case '-':
        case '_':
            camera.zoom(0.6f);
            break;
        case 27:
#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
            glutLeaveMainLoop();
#else
            std::exit(0);
#endif
            return;
        default:
            handled = false;
            break;
    }

    applyTorqueInput();

    if(handled)
    {
        g_app->requestRedisplay();
    }
}

void Input::keyboardUp(unsigned char key, int, int)
{
    if(g_app == nullptr)
    {
        return;
    }

    switch(key)
    {
        case 'q':
        case 'Q':
            g_shoulderPositive = false;
            break;
        case 'a':
        case 'A':
            g_shoulderNegative = false;
            break;
        case 'w':
        case 'W':
            g_elbowPositive = false;
            break;
        case 's':
        case 'S':
            g_elbowNegative = false;
            break;
        default:
            return;
    }

    applyTorqueInput();
    g_app->requestRedisplay();
}

void Input::mouseButton(int button, int state, int x, int y)
{
    if(g_app == nullptr)
    {
        return;
    }

    g_app->getCamera().onMouseButton(button, state, x, y);
    g_app->requestRedisplay();
}

void Input::mouseMove(int x, int y)
{
    if(g_app == nullptr)
    {
        return;
    }

    g_app->getCamera().onMouseMove(x, y);
    g_app->requestRedisplay();
}