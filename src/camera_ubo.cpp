#include "camera_ubo.hpp"
#include <iostream>

CameraUBO::CameraUBO() : m_UBO(0), m_initialized(false) {
}

CameraUBO::~CameraUBO() {
    Cleanup();
}

bool CameraUBO::Init() {
    if (m_initialized) return true;

    glGenBuffers(1, &m_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraMatrices), nullptr, GL_DYNAMIC_DRAW);
    
    glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT, m_UBO);
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    m_initialized = true;
    std::cout << "Camera UBO initialized successfully" << std::endl;
    
    return true;
}

void CameraUBO::UpdateMatrices(const Camera& camera, float aspect, float near, float far) {
    if (!m_initialized) return;

    m_matrices.view = camera.GetViewMatrix();
    m_matrices.projection = glm::perspective(glm::radians(camera.Zoom), aspect, near, far);
    m_matrices.viewPosition = camera.Position;

    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraMatrices), &m_matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CameraUBO::BindToPoint(unsigned int bindingPoint) {
    if (!m_initialized) return;
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_UBO);
}

void CameraUBO::Cleanup() {
    if (m_initialized && m_UBO != 0) {
        glDeleteBuffers(1, &m_UBO);
        m_UBO = 0;
        m_initialized = false;
    }
}
