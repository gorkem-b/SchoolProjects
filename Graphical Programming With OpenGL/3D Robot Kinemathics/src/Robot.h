#ifndef ROBOT_H
#define ROBOT_H

class Robot
{
public:
    enum ScenarioPreset
    {
        ScenarioFreeSwing = 1,
        ScenarioDampedRelease = 2,
        ScenarioTorqueControl = 3
    };

    struct Vec2
    {
        float x;
        float y;
    };

    Robot();

    void update(float deltaSeconds);
    void draw() const;

    void resetCurrentScenario();
    void setScenario(ScenarioPreset preset);
    void setControlInput(float shoulderAxis, float elbowAxis);

    float getShoulderAngleDegrees() const;
    float getElbowAngleDegrees() const;
    float getShoulderVelocityDegrees() const;
    float getElbowVelocityDegrees() const;
    float getShoulderTorque() const;
    float getElbowTorque() const;
    float getKineticEnergy() const;
    float getPotentialEnergy() const;
    float getTotalEnergy() const;
    const char* getScenarioName() const;
    bool hasActiveCollision() const;
    float getLastCollisionPenetration() const;
    void toggleDebugDraw();
    bool isDebugDrawEnabled() const;

private:
    struct LinkParameters
    {
        float mass;
        float length;
        float centerOfMass;
        float inertia;
        float damping;
        float minAngle;
        float maxAngle;
        float thickness;
    };

    static float clamp(float value, float minValue, float maxValue);
    static float degreesToRadians(float degrees);
    static float radiansToDegrees(float radians);

    void initializePhysicalParameters();
    void applyScenarioState(ScenarioPreset preset);
    void applyJointLimits();
    void resolveSelfCollision();
    bool resolveLinkContact(const Vec2& base, const Vec2& joint, const Vec2& tip, float radius, bool forearm);
    bool computeTorsoCorrection(const Vec2& point, float radius, Vec2& correction, float& penetration) const;
    void applyUpperArmCorrection(const Vec2& samplePoint, const Vec2& correction);
    void applyForearmCorrection(const Vec2& elbowPoint, const Vec2& samplePoint, const Vec2& correction);
    Vec2 getShoulderPosition() const;
    Vec2 getElbowPosition() const;
    Vec2 getWristPosition() const;

    void drawBox(float width, float height, float depth) const;
    void drawTorso() const;
    void drawHead() const;
    void drawStaticLeftArm() const;
    void drawStaticLegs() const;
    void drawDynamicRightArm() const;
    void drawJointSphere(float radius) const;
    void drawDebugVolumes() const;

    LinkParameters m_upperArm;
    LinkParameters m_forearm;
    ScenarioPreset m_currentScenario;
    float m_gravity;
    float m_maxShoulderTorque;
    float m_maxElbowTorque;
    float m_shoulderInputAxis;
    float m_elbowInputAxis;
    float m_shoulderAngle;
    float m_elbowAngle;
    float m_shoulderVelocity;
    float m_elbowVelocity;
    bool m_upperArmContact;
    bool m_forearmContact;
    float m_lastCollisionPenetration;
    bool m_debugDrawEnabled;
};

#endif