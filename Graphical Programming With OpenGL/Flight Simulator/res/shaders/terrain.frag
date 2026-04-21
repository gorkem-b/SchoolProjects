#version 330 core

in VS_OUT
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 worldTangent;
    vec2 texCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform vec3 lightDirection;
uniform vec3 sunColor;
uniform vec3 skyAmbientColor;
uniform vec3 groundAmbientColor;
uniform vec3 cameraPosition;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

vec3 GammaEncode(vec3 color)
{
    return pow(max(color, vec3(0.0)), vec3(1.0 / 2.2));
}

void main()
{
    vec3 baseColor = texture(diffuseTexture, fs_in.texCoord).rgb;
    vec3 geometryNormal = normalize(fs_in.worldNormal);
    vec3 tangent = normalize(fs_in.worldTangent - geometryNormal * dot(geometryNormal, fs_in.worldTangent));
    vec3 bitangent = normalize(cross(geometryNormal, tangent));
    vec3 tangentNormal = texture(normalTexture, fs_in.texCoord).xyz * 2.0 - 1.0;
    vec3 normal = normalize(mat3(tangent, bitangent, geometryNormal) * tangentNormal);

    float slope = 1.0 - clamp(dot(normal, vec3(0.0, 1.0, 0.0)), 0.0, 1.0);
    float mountainBlend = smoothstep(35.0, 140.0, fs_in.worldPosition.y);
    float rockBlend = smoothstep(0.20, 0.75, slope);
    vec3 vegetationTint = vec3(0.84, 0.98, 0.84);
    vec3 rockTint = vec3(0.72, 0.70, 0.68);
    vec3 snowTint = vec3(1.08, 1.08, 1.12);
    vec3 terrainTint = mix(vegetationTint, rockTint, rockBlend);
    terrainTint = mix(terrainTint, snowTint, mountainBlend);
    baseColor *= terrainTint;

    vec3 lightDir = normalize(-lightDirection);
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 viewDirection = normalize(cameraPosition - fs_in.worldPosition);
    vec3 halfVector = normalize(viewDirection + lightDir);
    float specular = pow(max(dot(normal, halfVector), 0.0), 24.0) * 0.18;
    vec3 ambient = mix(groundAmbientColor, skyAmbientColor, clamp(normal.y * 0.5 + 0.5, 0.0, 1.0));
    float backScatter = pow(max(dot(viewDirection, -lightDir), 0.0), 3.0) * 0.08;

    vec3 litColor = baseColor * (ambient + diffuse * 0.85 * sunColor) + sunColor * (specular + backScatter);

    float distanceToCamera = distance(cameraPosition, fs_in.worldPosition);
    float fogFactor = clamp((fogFar - distanceToCamera) / (fogFar - fogNear), 0.0, 1.0);
    vec3 color = mix(fogColor, litColor, fogFactor);

    FragColor = vec4(GammaEncode(color), 1.0);
}