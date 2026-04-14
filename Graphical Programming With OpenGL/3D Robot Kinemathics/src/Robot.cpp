#include "Robot.h"

#include <GL/glut.h>

#include <cmath>

namespace
{
    const float PI = 3.14159265358979323846f;

    Robot::Vec2 makeVec2(float x, float y)
    {
        Robot::Vec2 result;
        result.x = x;
        result.y = y;
        return result;
    }

    Robot::Vec2 add(const Robot::Vec2& a, const Robot::Vec2& b)
    {
        return makeVec2(a.x + b.x, a.y + b.y);
    }

    Robot::Vec2 subtract(const Robot::Vec2& a, const Robot::Vec2& b)
    {
        return makeVec2(a.x - b.x, a.y - b.y);
    }

    Robot::Vec2 scale(const Robot::Vec2& value, float factor)
    {
        return makeVec2(value.x * factor, value.y * factor);
    }

    float lengthSquared(const Robot::Vec2& value)
    {
        return value.x * value.x + value.y * value.y;
    }

    float cross2D(const Robot::Vec2& a, const Robot::Vec2& b)
    {
        return a.x * b.y - a.y * b.x;
    }
}

Robot::Robot()
    : m_currentScenario(ScenarioFreeSwing)
    , m_gravity(9.81f)
    , m_maxShoulderTorque(42.0f)
    , m_maxElbowTorque(24.0f)
    , m_shoulderInputAxis(0.0f)
    , m_elbowInputAxis(0.0f)
    , m_shoulderAngle(0.0f)
    , m_elbowAngle(0.0f)
    , m_shoulderVelocity(0.0f)
    , m_elbowVelocity(0.0f)
    , m_upperArmContact(false)
    , m_forearmContact(false)
    , m_lastCollisionPenetration(0.0f)
    , m_debugDrawEnabled(false)
{
    initializePhysicalParameters();
    applyScenarioState(m_currentScenario);
}

void Robot::update(float deltaSeconds)
{
    const float cosElbow = std::cos(m_elbowAngle);
    const float sinElbow = std::sin(m_elbowAngle);

    const float m11 = m_upperArm.inertia + m_forearm.inertia
        + m_upperArm.mass * m_upperArm.centerOfMass * m_upperArm.centerOfMass
        + m_forearm.mass * (m_upperArm.length * m_upperArm.length
            + m_forearm.centerOfMass * m_forearm.centerOfMass
            + 2.0f * m_upperArm.length * m_forearm.centerOfMass * cosElbow);
    const float m12 = m_forearm.inertia
        + m_forearm.mass * (m_forearm.centerOfMass * m_forearm.centerOfMass
            + m_upperArm.length * m_forearm.centerOfMass * cosElbow);
    const float m22 = m_forearm.inertia + m_forearm.mass * m_forearm.centerOfMass * m_forearm.centerOfMass;

    const float coupling = m_forearm.mass * m_upperArm.length * m_forearm.centerOfMass * sinElbow;
    const float coriolis1 = -coupling * (2.0f * m_shoulderVelocity * m_elbowVelocity + m_elbowVelocity * m_elbowVelocity);
    const float coriolis2 = coupling * m_shoulderVelocity * m_shoulderVelocity;

    const float gravity1 = ((m_upperArm.mass * m_upperArm.centerOfMass)
        + (m_forearm.mass * m_upperArm.length)) * m_gravity * std::cos(m_shoulderAngle)
        + m_forearm.mass * m_forearm.centerOfMass * m_gravity * std::cos(m_shoulderAngle + m_elbowAngle);
    const float gravity2 = m_forearm.mass * m_forearm.centerOfMass * m_gravity * std::cos(m_shoulderAngle + m_elbowAngle);

    const float shoulderTorque = getShoulderTorque();
    const float elbowTorque = getElbowTorque();

    const float rhs1 = shoulderTorque - coriolis1 - gravity1 - m_upperArm.damping * m_shoulderVelocity;
    const float rhs2 = elbowTorque - coriolis2 - gravity2 - m_forearm.damping * m_elbowVelocity;

    const float determinant = m11 * m22 - m12 * m12;
    if(std::fabs(determinant) < 1.0e-5f)
    {
        return;
    }

    const float shoulderAcceleration = (rhs1 * m22 - rhs2 * m12) / determinant;
    const float elbowAcceleration = (m11 * rhs2 - m12 * rhs1) / determinant;

    m_shoulderVelocity += shoulderAcceleration * deltaSeconds;
    m_elbowVelocity += elbowAcceleration * deltaSeconds;
    m_shoulderAngle += m_shoulderVelocity * deltaSeconds;
    m_elbowAngle += m_elbowVelocity * deltaSeconds;

    applyJointLimits();
    resolveSelfCollision();
}

void Robot::draw() const
{
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);

    drawTorso();
    drawHead();
    drawStaticLeftArm();
    drawStaticLegs();
    drawDynamicRightArm();

    if(m_debugDrawEnabled)
    {
        drawDebugVolumes();
    }

    glPopMatrix();
}

void Robot::resetCurrentScenario()
{
    applyScenarioState(m_currentScenario);
}

void Robot::setScenario(ScenarioPreset preset)
{
    m_currentScenario = preset;
    applyScenarioState(preset);
}

void Robot::setControlInput(float shoulderAxis, float elbowAxis)
{
    m_shoulderInputAxis = clamp(shoulderAxis, -1.0f, 1.0f);
    m_elbowInputAxis = clamp(elbowAxis, -1.0f, 1.0f);
}

float Robot::getShoulderAngleDegrees() const
{
    return radiansToDegrees(m_shoulderAngle);
}

float Robot::getElbowAngleDegrees() const
{
    return radiansToDegrees(m_elbowAngle);
}

float Robot::getShoulderVelocityDegrees() const
{
    return radiansToDegrees(m_shoulderVelocity);
}

float Robot::getElbowVelocityDegrees() const
{
    return radiansToDegrees(m_elbowVelocity);
}

float Robot::getShoulderTorque() const
{
    return m_shoulderInputAxis * m_maxShoulderTorque;
}

float Robot::getElbowTorque() const
{
    return m_elbowInputAxis * m_maxElbowTorque;
}

float Robot::getKineticEnergy() const
{
    const float cosElbow = std::cos(m_elbowAngle);
    const float m11 = m_upperArm.inertia + m_forearm.inertia
        + m_upperArm.mass * m_upperArm.centerOfMass * m_upperArm.centerOfMass
        + m_forearm.mass * (m_upperArm.length * m_upperArm.length
            + m_forearm.centerOfMass * m_forearm.centerOfMass
            + 2.0f * m_upperArm.length * m_forearm.centerOfMass * cosElbow);
    const float m12 = m_forearm.inertia
        + m_forearm.mass * (m_forearm.centerOfMass * m_forearm.centerOfMass
            + m_upperArm.length * m_forearm.centerOfMass * cosElbow);
    const float m22 = m_forearm.inertia + m_forearm.mass * m_forearm.centerOfMass * m_forearm.centerOfMass;

    return 0.5f * (m11 * m_shoulderVelocity * m_shoulderVelocity
        + 2.0f * m12 * m_shoulderVelocity * m_elbowVelocity
        + m22 * m_elbowVelocity * m_elbowVelocity);
}

float Robot::getPotentialEnergy() const
{
    const float upperHeight = m_upperArm.centerOfMass * (std::sin(m_shoulderAngle) + 1.0f);
    const float forearmHeight = m_upperArm.length * (std::sin(m_shoulderAngle) + 1.0f)
        + m_forearm.centerOfMass * (std::sin(m_shoulderAngle + m_elbowAngle) + 1.0f);

    return m_upperArm.mass * m_gravity * upperHeight
        + m_forearm.mass * m_gravity * forearmHeight;
}

float Robot::getTotalEnergy() const
{
    return getKineticEnergy() + getPotentialEnergy();
}

const char* Robot::getScenarioName() const
{
    switch(m_currentScenario)
    {
        case ScenarioFreeSwing:
            return "Free swing";
        case ScenarioDampedRelease:
            return "Damped release";
        case ScenarioTorqueControl:
            return "Torque control";
        default:
            return "Unknown";
    }
}

bool Robot::hasActiveCollision() const
{
    return m_upperArmContact || m_forearmContact;
}

float Robot::getLastCollisionPenetration() const
{
    return m_lastCollisionPenetration;
}

void Robot::toggleDebugDraw()
{
    m_debugDrawEnabled = !m_debugDrawEnabled;
}

bool Robot::isDebugDrawEnabled() const
{
    return m_debugDrawEnabled;
}

float Robot::clamp(float value, float minValue, float maxValue)
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

float Robot::degreesToRadians(float degrees)
{
    return degrees * PI / 180.0f;
}

float Robot::radiansToDegrees(float radians)
{
    return radians * 180.0f / PI;
}

void Robot::initializePhysicalParameters()
{
    m_upperArm.mass = 2.6f;
    m_upperArm.length = 1.55f;
    m_upperArm.centerOfMass = m_upperArm.length * 0.5f;
    m_upperArm.inertia = (m_upperArm.mass * m_upperArm.length * m_upperArm.length) / 12.0f;
    m_upperArm.damping = 5.5f;
    m_upperArm.minAngle = degreesToRadians(-170.0f);
    m_upperArm.maxAngle = degreesToRadians(70.0f);
    m_upperArm.thickness = 0.34f;

    m_forearm.mass = 1.8f;
    m_forearm.length = 1.25f;
    m_forearm.centerOfMass = m_forearm.length * 0.5f;
    m_forearm.inertia = (m_forearm.mass * m_forearm.length * m_forearm.length) / 12.0f;
    m_forearm.damping = 2.8f;
    m_forearm.minAngle = degreesToRadians(0.0f);
    m_forearm.maxAngle = degreesToRadians(150.0f);
    m_forearm.thickness = 0.26f;
}

void Robot::applyScenarioState(ScenarioPreset preset)
{
    m_shoulderInputAxis = 0.0f;
    m_elbowInputAxis = 0.0f;
    m_upperArmContact = false;
    m_forearmContact = false;
    m_lastCollisionPenetration = 0.0f;

    switch(preset)
    {
        case ScenarioFreeSwing:
            m_shoulderAngle = degreesToRadians(-40.0f);
            m_elbowAngle = degreesToRadians(35.0f);
            m_shoulderVelocity = 0.0f;
            m_elbowVelocity = 0.0f;
            break;
        case ScenarioDampedRelease:
            m_shoulderAngle = degreesToRadians(10.0f);
            m_elbowAngle = degreesToRadians(85.0f);
            m_shoulderVelocity = degreesToRadians(18.0f);
            m_elbowVelocity = degreesToRadians(-10.0f);
            break;
        case ScenarioTorqueControl:
        default:
            m_shoulderAngle = degreesToRadians(-95.0f);
            m_elbowAngle = degreesToRadians(25.0f);
            m_shoulderVelocity = 0.0f;
            m_elbowVelocity = 0.0f;
            break;
    }

    applyJointLimits();
}

void Robot::applyJointLimits()
{
    if(m_shoulderAngle < m_upperArm.minAngle)
    {
        m_shoulderAngle = m_upperArm.minAngle;
        if(m_shoulderVelocity < 0.0f)
        {
            m_shoulderVelocity = 0.0f;
        }
    }
    else if(m_shoulderAngle > m_upperArm.maxAngle)
    {
        m_shoulderAngle = m_upperArm.maxAngle;
        if(m_shoulderVelocity > 0.0f)
        {
            m_shoulderVelocity = 0.0f;
        }
    }

    if(m_elbowAngle < m_forearm.minAngle)
    {
        m_elbowAngle = m_forearm.minAngle;
        if(m_elbowVelocity < 0.0f)
        {
            m_elbowVelocity = 0.0f;
        }
    }
    else if(m_elbowAngle > m_forearm.maxAngle)
    {
        m_elbowAngle = m_forearm.maxAngle;
        if(m_elbowVelocity > 0.0f)
        {
            m_elbowVelocity = 0.0f;
        }
    }
}

void Robot::resolveSelfCollision()
{
    m_upperArmContact = false;
    m_forearmContact = false;
    m_lastCollisionPenetration = 0.0f;

    for(int iteration = 0; iteration < 3; ++iteration)
    {
        const Vec2 shoulder = getShoulderPosition();
        const Vec2 elbow = getElbowPosition();
        const Vec2 wrist = getWristPosition();

        const bool upperHit = resolveLinkContact(shoulder, elbow, elbow, m_upperArm.thickness * 0.58f, false);
        const bool foreHit = resolveLinkContact(shoulder, elbow, wrist, m_forearm.thickness * 0.60f, true);

        applyJointLimits();

        if(!upperHit && !foreHit)
        {
            break;
        }
    }
}

bool Robot::resolveLinkContact(const Vec2& base, const Vec2& joint, const Vec2& tip, float radius, bool forearm)
{
    static const float upperSamples[] = { 0.35f, 0.60f, 0.85f };
    static const float forearmSamples[] = { 0.25f, 0.55f, 0.85f };

    const float* fractions = forearm ? forearmSamples : upperSamples;
    const Vec2 segmentStart = forearm ? joint : base;
    const Vec2 segmentVector = forearm ? subtract(tip, joint) : subtract(joint, base);
    bool touched = false;

    for(int i = 0; i < 3; ++i)
    {
        const Vec2 samplePoint = add(segmentStart, scale(segmentVector, fractions[i]));
        Vec2 correction = makeVec2(0.0f, 0.0f);
        float penetration = 0.0f;
        if(!computeTorsoCorrection(samplePoint, radius, correction, penetration))
        {
            continue;
        }

        touched = true;
        if(penetration > m_lastCollisionPenetration)
        {
            m_lastCollisionPenetration = penetration;
        }

        if(forearm)
        {
            m_forearmContact = true;
            applyForearmCorrection(joint, samplePoint, correction);
        }
        else
        {
            m_upperArmContact = true;
            applyUpperArmCorrection(samplePoint, correction);
        }
    }

    return touched;
}

bool Robot::computeTorsoCorrection(const Vec2& point, float radius, Vec2& correction, float& penetration) const
{
    const float halfWidth = 1.0f + radius;
    const float halfHeight = 1.1f + radius;

    if(std::fabs(point.x) >= halfWidth || std::fabs(point.y) >= halfHeight)
    {
        return false;
    }

    const float pushX = halfWidth - std::fabs(point.x);
    const float pushY = halfHeight - std::fabs(point.y);

    if(pushX < pushY)
    {
        const float signX = (point.x >= 0.0f) ? 1.0f : -1.0f;
        correction = makeVec2(signX * pushX, 0.0f);
        penetration = pushX;
    }
    else
    {
        const float signY = (point.y >= 0.0f) ? 1.0f : -1.0f;
        correction = makeVec2(0.0f, signY * pushY);
        penetration = pushY;
    }

    return true;
}

void Robot::applyUpperArmCorrection(const Vec2& samplePoint, const Vec2& correction)
{
    const Vec2 shoulder = getShoulderPosition();
    const Vec2 leverArm = subtract(samplePoint, shoulder);
    const float denom = lengthSquared(leverArm) + 1.0e-4f;
    const float deltaAngle = 0.75f * cross2D(leverArm, correction) / denom;

    m_shoulderAngle += deltaAngle;
    m_shoulderVelocity *= 0.35f;
}

void Robot::applyForearmCorrection(const Vec2& elbowPoint, const Vec2& samplePoint, const Vec2& correction)
{
    const Vec2 shoulder = getShoulderPosition();
    const Vec2 shoulderLever = subtract(samplePoint, shoulder);
    const Vec2 elbowLever = subtract(samplePoint, elbowPoint);

    const float shoulderDelta = 0.28f * cross2D(shoulderLever, correction) / (lengthSquared(shoulderLever) + 1.0e-4f);
    const float elbowDelta = 0.95f * cross2D(elbowLever, correction) / (lengthSquared(elbowLever) + 1.0e-4f);

    m_shoulderAngle += shoulderDelta;
    m_elbowAngle += elbowDelta;
    m_shoulderVelocity *= 0.55f;
    m_elbowVelocity *= 0.25f;
}

Robot::Vec2 Robot::getShoulderPosition() const
{
    return makeVec2(1.05f, 0.75f);
}

Robot::Vec2 Robot::getElbowPosition() const
{
    const Vec2 shoulder = getShoulderPosition();
    return add(shoulder, makeVec2(std::cos(m_shoulderAngle) * m_upperArm.length,
                                  std::sin(m_shoulderAngle) * m_upperArm.length));
}

Robot::Vec2 Robot::getWristPosition() const
{
    const Vec2 elbow = getElbowPosition();
    const float absoluteForearmAngle = m_shoulderAngle + m_elbowAngle;
    return add(elbow, makeVec2(std::cos(absoluteForearmAngle) * m_forearm.length,
                               std::sin(absoluteForearmAngle) * m_forearm.length));
}

void Robot::drawBox(float width, float height, float depth) const
{
    glPushMatrix();
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void Robot::drawTorso() const
{
    glColor3f(0.28f, 0.60f, 0.96f);
    drawBox(2.0f, 2.2f, 1.0f);
}

void Robot::drawHead() const
{
    glPushMatrix();
    glTranslatef(0.0f, 1.65f, 0.0f);
    glColor3f(0.90f, 0.90f, 0.92f);
    drawBox(0.9f, 0.9f, 0.9f);
    glPopMatrix();
}

void Robot::drawStaticLeftArm() const
{
    glPushMatrix();
    glTranslatef(-1.05f, 0.75f, 0.0f);
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(0.80f, 0.52f, 0.18f);
    glTranslatef(m_upperArm.length * 0.5f, 0.0f, 0.0f);
    drawBox(m_upperArm.length, m_upperArm.thickness, m_upperArm.thickness);
    glTranslatef(m_upperArm.length * 0.5f, 0.0f, 0.0f);
    drawJointSphere(0.10f);

    glTranslatef(m_forearm.length * 0.5f, 0.0f, 0.0f);
    glColor3f(0.88f, 0.66f, 0.26f);
    drawBox(m_forearm.length, m_forearm.thickness, m_forearm.thickness);
    glPopMatrix();
}

void Robot::drawStaticLegs() const
{
    const float upperLegLength = 1.0f;
    const float lowerLegLength = 0.95f;
    const float upperLegThickness = 0.48f;
    const float lowerLegThickness = 0.38f;

    for(int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(0.43f * static_cast<float>(side), -1.05f, 0.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(0.24f, 0.76f, 0.42f);
        glTranslatef(upperLegLength * 0.5f, 0.0f, 0.0f);
        drawBox(upperLegLength, upperLegThickness, upperLegThickness);
        glTranslatef(upperLegLength * 0.5f, 0.0f, 0.0f);
        glColor3f(0.18f, 0.66f, 0.36f);
        glTranslatef(lowerLegLength * 0.5f, 0.0f, 0.0f);
        drawBox(lowerLegLength, lowerLegThickness, lowerLegThickness);
        glPopMatrix();
    }
}

void Robot::drawDynamicRightArm() const
{
    glPushMatrix();
    glTranslatef(1.05f, 0.75f, 0.0f);

    glColor3f(0.92f, 0.52f, 0.18f);
    drawJointSphere(0.12f);

    glRotatef(radiansToDegrees(m_shoulderAngle), 0.0f, 0.0f, 1.0f);
    glTranslatef(m_upperArm.length * 0.5f, 0.0f, 0.0f);
    glColor3f(0.95f, 0.64f, 0.20f);
    drawBox(m_upperArm.length, m_upperArm.thickness, m_upperArm.thickness);
    glTranslatef(m_upperArm.length * 0.5f, 0.0f, 0.0f);

    glColor3f(0.98f, 0.78f, 0.28f);
    drawJointSphere(0.10f);

    glRotatef(radiansToDegrees(m_elbowAngle), 0.0f, 0.0f, 1.0f);
    glTranslatef(m_forearm.length * 0.5f, 0.0f, 0.0f);
    glColor3f(0.94f, 0.78f, 0.34f);
    drawBox(m_forearm.length, m_forearm.thickness, m_forearm.thickness);
    glTranslatef(m_forearm.length * 0.5f, 0.0f, 0.0f);

    glColor3f(0.96f, 0.86f, 0.58f);
    drawJointSphere(0.08f);
    glPopMatrix();
}

void Robot::drawJointSphere(float radius) const
{
    glutSolidSphere(radius, 18, 18);
}

void Robot::drawDebugVolumes() const
{
    const Vec2 shoulder = getShoulderPosition();
    const Vec2 elbow = getElbowPosition();
    const Vec2 wrist = getWristPosition();

    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);

    glColor3f(0.20f, 0.90f, 0.95f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.56f);
    glScalef(2.0f, 2.2f, 1.02f);
    glutWireCube(1.0f);
    glPopMatrix();

    glBegin(GL_LINES);
    glColor3f(m_upperArmContact ? 1.0f : 0.85f, m_upperArmContact ? 0.25f : 0.75f, 0.20f);
    glVertex3f(shoulder.x, shoulder.y, 0.58f);
    glVertex3f(elbow.x, elbow.y, 0.58f);

    glColor3f(m_forearmContact ? 1.0f : 0.95f, m_forearmContact ? 0.20f : 0.82f, 0.20f);
    glVertex3f(elbow.x, elbow.y, 0.58f);
    glVertex3f(wrist.x, wrist.y, 0.58f);
    glEnd();

    glPushMatrix();
    glTranslatef(shoulder.x, shoulder.y, 0.58f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glutWireSphere(m_upperArm.thickness * 0.58f, 12, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(elbow.x, elbow.y, 0.58f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glutWireSphere(m_forearm.thickness * 0.60f, 12, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(wrist.x, wrist.y, 0.58f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glutWireSphere(m_forearm.thickness * 0.60f, 12, 12);
    glPopMatrix();
}