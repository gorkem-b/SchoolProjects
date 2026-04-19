#version 330 core

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uSpecularMap;
uniform sampler2D uEmissionMap;
uniform sampler2D uCloudMap;
uniform sampler2D uOpacityMap;

uniform bool uHasSpecularMap;
uniform bool uHasEmissionMap;
uniform bool uHasCloudMap;
uniform bool uHasOpacityMap;
uniform bool uIsEmissive;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 uTint;
uniform vec3 uAtmosphereColor;

uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uSpecularStrength;
uniform float uShininess;
uniform float uEmissionStrength;
uniform float uAtmosphereStrength;
uniform float uCloudOpacity;
uniform float uCloudSpeed;
uniform float uAlpha;
uniform float uTime;

void main()
{
    vec2 uv = fs_in.TexCoord;
    vec4 baseSample = texture(uTexture, uv);
    vec3 baseColor = pow(baseSample.rgb * uTint, vec3(2.2));
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    float specularMask = uHasSpecularMap ? texture(uSpecularMap, uv).r : 1.0;
    float specularHighlight = diffuseStrength > 0.0 ? pow(max(dot(viewDir, reflectDir), 0.0), uShininess) : 0.0;

    vec3 ambient = uAmbientStrength * baseColor;
    vec3 diffuse = uDiffuseStrength * diffuseStrength * baseColor;
    vec3 specular = uSpecularStrength * specularMask * specularHighlight * vec3(1.0);

    vec3 emission = vec3(0.0);
    if (uHasEmissionMap)
    {
        float nightMask = pow(clamp(1.0 - diffuseStrength, 0.0, 1.0), 2.0);
        emission += pow(texture(uEmissionMap, uv).rgb, vec3(2.2)) * uEmissionStrength * nightMask;
    }

    if (uIsEmissive)
    {
        emission += baseColor * uEmissionStrength;
        diffuse *= 0.25;
        specular *= 0.0;
        ambient += 0.35 * baseColor;
    }

    vec3 color = ambient + diffuse + specular + emission;

    if (uHasCloudMap)
    {
        vec2 cloudUv = vec2(fract(uv.x + uTime * uCloudSpeed), uv.y);
        vec4 cloudSample = texture(uCloudMap, cloudUv);
        float cloudAlpha = cloudSample.a > 0.0 ? cloudSample.a : dot(cloudSample.rgb, vec3(0.3333));
        cloudAlpha *= uCloudOpacity;
        vec3 cloudColor = pow(cloudSample.rgb, vec3(2.2));
        vec3 cloudLighting = cloudColor * (0.3 + 0.7 * diffuseStrength);
        color = mix(color, color + cloudLighting, clamp(cloudAlpha, 0.0, 1.0));
    }

    float rim = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);
    vec3 atmosphere = uAtmosphereColor * rim * uAtmosphereStrength;
    color += atmosphere;

    float alpha = uAlpha * baseSample.a;
    if (uHasOpacityMap)
    {
        alpha *= texture(uOpacityMap, uv).r;
    }

    if (alpha < 0.05)
    {
        discard;
    }

    vec3 mapped = color / (color + vec3(1.0));
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, alpha);
}