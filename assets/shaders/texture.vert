#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform CameraMatrices {
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
};

layout (std140) uniform TransformMatrices {
    mat4 model;
    mat4 normalMatrix;
};

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
