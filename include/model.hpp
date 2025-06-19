#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shader.hpp"

struct MaterialInfo {
    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);
    bool hasTexture = false;
    unsigned int textureID = 0;
};

struct Texture {
    GLuint id;
};

class Model {
public:
    Model(const std::string& path);
    void Draw(const Shader& shader);

private:
    unsigned int VAO, VBO;
    size_t vertexCount;
    std::vector<Texture> textures;
    void loadModel(const std::string& path);
    std::vector<MaterialInfo> materialsInfo;
    MaterialInfo material;
};

#endif
