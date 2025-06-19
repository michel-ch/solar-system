#ifndef CAMERA_UBO_HPP
#define CAMERA_UBO_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"

class CameraUBO {
public:
    struct CameraMatrices {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
        alignas(16) glm::vec3 viewPosition;
    };

    CameraUBO();
    ~CameraUBO();
    
    bool Init();
    void UpdateMatrices(const Camera& camera, float aspect, float near = 0.1f, float far = 100.0f);
    void BindToPoint(unsigned int bindingPoint);
    void Cleanup();

private:
    unsigned int m_UBO;
    CameraMatrices m_matrices;
    bool m_initialized;
    static const unsigned int BINDING_POINT = 0;
};

#endif
