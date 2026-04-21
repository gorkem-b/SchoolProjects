#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 worldTangent;
    vec2 texCoord;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    vs_out.worldPosition = worldPosition.xyz;
    vs_out.worldNormal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.worldTangent = normalize(mat3(model) * vec3(1.0, 0.0, 0.0));
    vs_out.texCoord = aTexCoord;

    gl_Position = projection * view * worldPosition;
}