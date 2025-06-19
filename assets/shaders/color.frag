#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

layout (std140) uniform CameraMatrices {
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
};

uniform vec3 u_Color;
uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

void main() {
    vec3 lightDir = normalize(light_position - FragPos);
    vec3 viewDir = normalize(viewPosition - FragPos);
    
    vec3 ambient = light_ambient * u_Color;
    
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = light_diffuse * diff * u_Color;
    
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light_specular * spec;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
