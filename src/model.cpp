#include "model.hpp"
#include <tiny_obj_loader.h>
#include <iostream>
#include <stb_image.h>

Model::Model(const std::string& path) {
    loadModel(path);
}

unsigned int loadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        GLenum internalFormat;
        if (nrComponents == 1) {
            format = GL_RED;
            internalFormat = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
            internalFormat = GL_SRGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
            internalFormat = GL_SRGB_ALPHA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    return textureID;
}

void Model::loadModel(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materialsTinyObj;
    std::string warn, err;

    std::string basedir = path.substr(0, path.find_last_of('/') + 1);
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materialsTinyObj, &warn, &err, path.c_str(), basedir.c_str());

    if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
    if (!err.empty()) std::cerr << "ERR: " << err << std::endl;
    if (!ret) throw std::runtime_error("Failed to load OBJ file");

    if (!materialsTinyObj.empty()) {
        const auto& matTiny = materialsTinyObj[0];
        material.ambient = glm::vec3(matTiny.ambient[0], matTiny.ambient[1], matTiny.ambient[2]);
        material.diffuse = glm::vec3(matTiny.diffuse[0], matTiny.diffuse[1], matTiny.diffuse[2]);
        material.specular = glm::vec3(matTiny.specular[0], matTiny.specular[1], matTiny.specular[2]);

        if (!matTiny.diffuse_texname.empty()) {
            std::string texturePath = basedir + matTiny.diffuse_texname;
            material.textureID = loadTexture(texturePath);
            material.hasTexture = true;
        }
    }

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    std::vector<Vertex> vertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex v{};

            v.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (index.normal_index >= 0) {
                v.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            if (index.texcoord_index >= 0) {
                v.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            vertices.push_back(v);
        }
    }

    vertexCount = vertices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Model::Draw(const Shader& shader) {
    if (material.hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.textureID);
        shader.setInt("material.hasTexture", 1);
    } else {
        shader.setInt("material.hasTexture", 0);
        shader.setVec3("material.ambient", material.ambient);
        shader.setVec3("material.diffuse", material.diffuse);
        shader.setVec3("material.specular", material.specular);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}
