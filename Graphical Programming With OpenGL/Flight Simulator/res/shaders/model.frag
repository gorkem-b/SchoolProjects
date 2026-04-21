#version 330 core

in VS_OUT
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec2 texCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
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
    vec3 normal = normalize(fs_in.worldNormal);
    vec3 lightDir = normalize(-lightDirection);
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 viewDirection = normalize(cameraPosition - fs_in.worldPosition);
    vec3 reflectedLight = reflect(normalize(lightDirection), normal);
    float specular = pow(max(dot(viewDirection, reflectedLight), 0.0), 42.0) * 0.55;
    float fresnel = pow(1.0 - max(dot(normal, viewDirection), 0.0), 3.0) * 0.20;
    vec3 ambient = mix(groundAmbientColor, skyAmbientColor, clamp(normal.y * 0.5 + 0.5, 0.0, 1.0));
    float rim = pow(1.0 - max(dot(normal, viewDirection), 0.0), 2.0) * 0.25;

    vec3 litColor = baseColor * (ambient + diffuse * 0.95 * sunColor) + sunColor * (specular + fresnel + rim * 0.3);

    float distanceToCamera = distance(cameraPosition, fs_in.worldPosition);
    float fogFactor = clamp((fogFar - distanceToCamera) / (fogFar - fogNear), 0.0, 1.0);
    vec3 color = mix(fogColor, litColor, fogFactor);

    FragColor = vec4(GammaEncode(color), 1.0);
}