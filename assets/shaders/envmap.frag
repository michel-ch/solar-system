#version 330 core
in vec3 WorldPos;
in vec3 Normal;

uniform samplerCube skybox;
uniform vec3 cameraPos;

out vec4 FragColor;

void main()
{
    vec3 I = normalize(WorldPos - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = texture(skybox, R);
}
