#version 330 core

in vec2 vUv;

out vec4 FragColor;

uniform float uAspectRatio;
uniform float uAltitude;
uniform float uSpeed;
uniform float uIAS;
uniform float uThrottle;
uniform float uAoA;
uniform float uGForce;
uniform float uPitch;
uniform float uBank;
uniform float uFpmX;
uniform float uFpmY;
uniform float uTrim;
uniform float uPaused;

mat2 rotation2d(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, -s, s, c);
}

float lineSegment(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

float plotLine(vec2 p, vec2 a, vec2 b, float thickness)
{
    return 1.0 - smoothstep(thickness, thickness + 0.0025, lineSegment(p, a, b));
}

float boxOutline(vec2 p, vec2 center, vec2 halfSize, float thickness)
{
    vec2 d = abs(p - center) - halfSize;
    float outside = length(max(d, 0.0));
    float inside = min(max(d.x, d.y), 0.0);
    float dist = outside + inside;
    return 1.0 - smoothstep(thickness, thickness + 0.0025, abs(dist));
}

void main()
{
    vec2 p = vUv * 2.0 - 1.0;
    p.x *= uAspectRatio;

    vec3 hudColor = vec3(0.18, 1.0, 0.62);
    float alpha = 0.0;

    vec2 ladderPoint = p;
    ladderPoint = rotation2d(radians(-uBank)) * ladderPoint;

    alpha += plotLine(p, vec2(-0.03, 0.0), vec2(0.03, 0.0), 0.0025);
    alpha += plotLine(p, vec2(0.0, -0.03), vec2(0.0, 0.03), 0.0025);
    alpha += boxOutline(p, vec2(0.0, 0.0), vec2(0.11, 0.07), 0.0025);

    float ladderCenter = clamp((-uAoA - uPitch * 0.65) / 25.0, -0.32, 0.32);
    for (int i = -3; i <= 3; ++i)
    {
        float y = ladderCenter + float(i) * 0.08;
        float width = (i == 0) ? 0.22 : 0.16;
        alpha += plotLine(ladderPoint, vec2(-width, y), vec2(width, y), 0.0022);
    }

    vec2 fpm = vec2(uFpmX * uAspectRatio, uFpmY);
    alpha += plotLine(p, fpm + vec2(-0.020, 0.0), fpm + vec2(-0.006, 0.0), 0.0025);
    alpha += plotLine(p, fpm + vec2(0.006, 0.0), fpm + vec2(0.020, 0.0), 0.0025);
    alpha += plotLine(p, fpm + vec2(0.0, -0.020), fpm + vec2(0.0, -0.006), 0.0025);
    alpha += plotLine(p, fpm + vec2(0.0, 0.006), fpm + vec2(0.0, 0.020), 0.0025);

    float throttleBarTop = -0.72 + uThrottle * 0.34;
    alpha += plotLine(p, vec2(-uAspectRatio + 0.16, -0.72), vec2(-uAspectRatio + 0.16, -0.38), 0.003);
    alpha += plotLine(p, vec2(-uAspectRatio + 0.13, throttleBarTop), vec2(-uAspectRatio + 0.19, throttleBarTop), 0.004);

    float speedMarker = clamp((uSpeed - 150.0) / 400.0, -0.18, 0.18);
    alpha += plotLine(p, vec2(uAspectRatio - 0.16, -0.72), vec2(uAspectRatio - 0.16, -0.38), 0.003);
    alpha += plotLine(p, vec2(uAspectRatio - 0.19, -0.55 + speedMarker), vec2(uAspectRatio - 0.13, -0.55 + speedMarker), 0.004);

    float iasMarker = clamp((uIAS - 150.0) / 400.0, -0.18, 0.18);
    alpha += plotLine(p, vec2(uAspectRatio - 0.40, -0.72), vec2(uAspectRatio - 0.40, -0.38), 0.003);
    alpha += plotLine(p, vec2(uAspectRatio - 0.43, -0.55 + iasMarker), vec2(uAspectRatio - 0.37, -0.55 + iasMarker), 0.004);

    float altitudeMarker = clamp((uAltitude - 150.0) / 600.0, -0.18, 0.18);
    alpha += plotLine(p, vec2(uAspectRatio - 0.28, -0.72), vec2(uAspectRatio - 0.28, -0.38), 0.003);
    alpha += plotLine(p, vec2(uAspectRatio - 0.31, -0.55 + altitudeMarker), vec2(uAspectRatio - 0.25, -0.55 + altitudeMarker), 0.004);

    float trimMarker = clamp(uTrim * 0.16, -0.12, 0.12);
    alpha += plotLine(p, vec2(-0.18, -0.82), vec2(0.18, -0.82), 0.003);
    alpha += plotLine(p, vec2(trimMarker - 0.01, -0.79), vec2(trimMarker + 0.01, -0.79), 0.004);

    float gMarker = clamp((uGForce - 1.0) / 6.0, -0.14, 0.14);
    alpha += plotLine(p, vec2(-uAspectRatio + 0.28, -0.72), vec2(-uAspectRatio + 0.28, -0.44), 0.003);
    alpha += plotLine(p, vec2(-uAspectRatio + 0.25, -0.58 + gMarker), vec2(-uAspectRatio + 0.31, -0.58 + gMarker), 0.004);

    if (uPaused > 0.5)
    {
        alpha += plotLine(p, vec2(-0.08, 0.72), vec2(-0.02, 0.72), 0.010);
        alpha += plotLine(p, vec2(0.02, 0.72), vec2(0.08, 0.72), 0.010);
    }

    alpha = clamp(alpha, 0.0, 0.95);
    FragColor = vec4(hudColor, alpha);
}