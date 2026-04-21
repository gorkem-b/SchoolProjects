#version 330 core

in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox;

vec3 GammaEncode(vec3 color)
{
    return pow(max(color, vec3(0.0)), vec3(1.0 / 2.2));
}

void main()
{
    vec3 dir = normalize(TexCoords);
    vec3 sampled = texture(skybox, dir).rgb;

    float horizon = 1.0 - abs(dir.y);
    float zenith = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);

    vec3 horizonTint = vec3(0.88, 0.73, 0.55);
    vec3 zenithTint = vec3(0.70, 0.82, 1.02);
    vec3 hazeTint = mix(horizonTint, zenithTint, zenith);

    sampled *= mix(vec3(0.94, 0.96, 1.02), vec3(1.02, 1.04, 1.08), zenith);
    sampled = mix(sampled, hazeTint, smoothstep(0.15, 0.85, horizon) * 0.22);

    FragColor = vec4(GammaEncode(sampled), 1.0);
}