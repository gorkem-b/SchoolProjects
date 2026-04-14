#include "App.h"

#include "Input.h"

#include <GL/glut.h>

#include <iomanip>
#include <iostream>
#include <sstream>

App* App::s_instance = nullptr;

App::App()
    : m_windowWidth(1280)
    , m_windowHeight(720)
    , m_lastElapsedTimeMs(0)
    , m_fixedDeltaTime(1.0f / 240.0f)
    , m_accumulator(0.0f)
    , m_lightingEnabled(true)
    , m_wireframeEnabled(false)
    , m_paused(false)
{
}

int App::run(int argc, char** argv)
{
    s_instance = this;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(m_windowWidth, m_windowHeight);
    glutCreateWindow("3D Robot Kinematics");

#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif

    initialize();

    glutDisplayFunc(App::displayCallback);
    glutReshapeFunc(App::reshapeCallback);
    glutIdleFunc(App::idleCallback);

    Input::bind(this);
    Input::registerCallbacks();
    Input::printControls();

    std::cout << "Application started. Close the window or press ESC to quit.\n";

    glutMainLoop();
    return 0;
}

void App::initialize()
{
    configureOpenGL();
    m_camera.setViewport(m_windowWidth, m_windowHeight);
    m_lastElapsedTimeMs = glutGet(GLUT_ELAPSED_TIME);
    m_accumulator = 0.0f;
}

void App::configureOpenGL()
{
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    const GLfloat globalAmbient[] = { 0.18f, 0.18f, 0.22f, 1.0f };
    const GLfloat lightAmbient[] = { 0.28f, 0.28f, 0.30f, 1.0f };
    const GLfloat lightDiffuse[] = { 0.92f, 0.92f, 0.88f, 1.0f };
    const GLfloat lightSpecular[] = { 1.0f, 1.0f, 0.98f, 1.0f };
    const GLfloat materialSpecular[] = { 0.35f, 0.35f, 0.35f, 1.0f };

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);
}

void App::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_camera.applyProjection();
    m_camera.applyView();

    applyLighting();

    if(m_wireframeEnabled)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    drawFloor(20.0f);
    m_robot.draw();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_LIGHTING);

    drawGrid(20, 1.0f);
    drawAxes(2.5f);
    drawOverlay();

    glutSwapBuffers();
}

void App::reshape(int width, int height)
{
    m_windowWidth = (width > 1) ? width : 1;
    m_windowHeight = (height > 1) ? height : 1;
    m_camera.setViewport(m_windowWidth, m_windowHeight);
}

void App::idle()
{
    const int currentElapsedTimeMs = glutGet(GLUT_ELAPSED_TIME);
    float deltaSeconds = static_cast<float>(currentElapsedTimeMs - m_lastElapsedTimeMs) * 0.001f;
    if(deltaSeconds < 0.0f)
    {
        deltaSeconds = 0.0f;
    }

    if(deltaSeconds > 0.10f)
    {
        deltaSeconds = 0.10f;
    }

    m_lastElapsedTimeMs = currentElapsedTimeMs;

    if(!m_paused)
    {
        m_accumulator += deltaSeconds;
        while(m_accumulator >= m_fixedDeltaTime)
        {
            m_robot.update(m_fixedDeltaTime);
            m_accumulator -= m_fixedDeltaTime;
        }
    }

    requestRedisplay();
}

void App::requestRedisplay() const
{
    glutPostRedisplay();
}

void App::toggleLighting()
{
    m_lightingEnabled = !m_lightingEnabled;
}

void App::toggleWireframe()
{
    m_wireframeEnabled = !m_wireframeEnabled;
}

void App::togglePause()
{
    m_paused = !m_paused;
}

bool App::isLightingEnabled() const
{
    return m_lightingEnabled;
}

bool App::isWireframeEnabled() const
{
    return m_wireframeEnabled;
}

bool App::isPaused() const
{
    return m_paused;
}

Robot& App::getRobot()
{
    return m_robot;
}

Camera& App::getCamera()
{
    return m_camera;
}

const Robot& App::getRobot() const
{
    return m_robot;
}

const Camera& App::getCamera() const
{
    return m_camera;
}

void App::applyLighting() const
{
    if(!m_lightingEnabled)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        return;
    }

    const GLfloat lightPosition[] = { 7.0f, 10.0f, 9.0f, 1.0f };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void App::drawFloor(float halfExtent) const
{
    glColor3f(0.16f, 0.18f, 0.22f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfExtent, 0.0f, -halfExtent);
    glVertex3f(-halfExtent, 0.0f, halfExtent);
    glVertex3f(halfExtent, 0.0f, halfExtent);
    glVertex3f(halfExtent, 0.0f, -halfExtent);
    glEnd();
}

void App::drawGrid(int halfExtent, float spacing) const
{
    const float gridY = 0.01f;

    glLineWidth(1.0f);
    glColor3f(0.22f, 0.24f, 0.28f);

    glBegin(GL_LINES);
    for(int i = -halfExtent; i <= halfExtent; ++i)
    {
        const float offset = static_cast<float>(i) * spacing;

        glVertex3f(offset, gridY, -halfExtent * spacing);
        glVertex3f(offset, gridY, halfExtent * spacing);

        glVertex3f(-halfExtent * spacing, gridY, offset);
        glVertex3f(halfExtent * spacing, gridY, offset);
    }
    glEnd();
}

void App::drawAxes(float length) const
{
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    glColor3f(0.95f, 0.20f, 0.20f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    glColor3f(0.20f, 0.95f, 0.20f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    glColor3f(0.20f, 0.45f, 0.95f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();
}

void App::drawOverlay() const
{
    std::ostringstream shoulderLine;
    shoulderLine << std::fixed << std::setprecision(1)
                 << "Shoulder: " << m_robot.getShoulderAngleDegrees() << " deg, "
                 << m_robot.getShoulderVelocityDegrees() << " deg/s, tau = "
                 << m_robot.getShoulderTorque() << " Nm";

    std::ostringstream elbowLine;
    elbowLine << std::fixed << std::setprecision(1)
              << "Elbow: " << m_robot.getElbowAngleDegrees() << " deg, "
              << m_robot.getElbowVelocityDegrees() << " deg/s, tau = "
              << m_robot.getElbowTorque() << " Nm";

    std::ostringstream energyLine;
    energyLine << std::fixed << std::setprecision(2)
               << "Energy  K = " << m_robot.getKineticEnergy()
               << ", P = " << m_robot.getPotentialEnergy()
               << ", T = " << m_robot.getTotalEnergy();

    std::ostringstream collisionLine;
    collisionLine << std::fixed << std::setprecision(3)
                  << "Collision: " << (m_robot.hasActiveCollision() ? "ACTIVE" : "clear")
                  << ", penetration = " << m_robot.getLastCollisionPenetration()
                  << ", debug = " << (m_robot.isDebugDrawEnabled() ? "on" : "off");

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(m_windowWidth), 0.0, static_cast<double>(m_windowHeight));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    int y = m_windowHeight - 24;
    glColor3f(0.96f, 0.96f, 0.88f);
    drawText2D(16, y, "3D Physics Figure Simulation - Two-Link Arm");
    y -= 18;
    drawText2D(16, y, std::string("Scenario: ") + m_robot.getScenarioName() + (m_paused ? " [PAUSED]" : " [RUNNING]"));
    y -= 18;
    drawText2D(16, y, shoulderLine.str());
    y -= 18;
    drawText2D(16, y, elbowLine.str());
    y -= 18;
    drawText2D(16, y, energyLine.str());
    y -= 18;
    drawText2D(16, y, collisionLine.str());
    y -= 18;
    drawText2D(16, y, "Q/A shoulder torque, W/S elbow torque, R reset, 1/2/3 scenarios, SPACE pause");
    y -= 18;
    drawText2D(16, y, "L lighting, M wireframe, B debug volumes, mouse orbit, +/- zoom, ESC quit");

    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void App::drawText2D(int x, int y, const std::string& text) const
{
    glRasterPos2i(x, y);
    for(std::string::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *it);
    }
}

void App::displayCallback()
{
    if(s_instance != nullptr)
    {
        s_instance->render();
    }
}

void App::reshapeCallback(int width, int height)
{
    if(s_instance != nullptr)
    {
        s_instance->reshape(width, height);
    }
}

void App::idleCallback()
{
    if(s_instance != nullptr)
    {
        s_instance->idle();
    }
}