#ifndef TRANSFORM_UBO_HPP
#define TRANSFORM_UBO_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TransformUBO {
public:
    struct TransformMatrices {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 normalMatrix;
    };

    TransformUBO();
    ~TransformUBO();
    
    bool Init();
    void UpdateTransform(const glm::mat4& modelMatrix);
    void BindToPoint(unsigned int bindingPoint);
    void Cleanup();

private:
    unsigned int m_UBO;
    TransformMatrices m_matrices;
    bool m_initialized;
    static const unsigned int BINDING_POINT = 1;
};

#endif
