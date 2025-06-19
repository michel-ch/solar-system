#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (std140) uniform CameraMatrices {
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
};

layout (std140) uniform TransformMatrices {
    mat4 model;
    mat4 normalMatrix;
};

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(mat3(normalMatrix) * aNormal);
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
