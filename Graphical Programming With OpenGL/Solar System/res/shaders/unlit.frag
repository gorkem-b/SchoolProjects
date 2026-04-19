#version 330 core

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    FragColor = vec4(texture(uTexture, fs_in.TexCoord).rgb, 1.0);
}