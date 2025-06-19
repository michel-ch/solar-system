#include "transform_ubo.hpp"
#include <iostream>

TransformUBO::TransformUBO() : m_UBO(0), m_initialized(false) {
}

TransformUBO::~TransformUBO() {
    Cleanup();
}

bool TransformUBO::Init() {
    if (m_initialized) return true;

    glGenBuffers(1, &m_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    
    glBufferData(GL_UNIFORM_BUFFER, sizeof(TransformMatrices), nullptr, GL_DYNAMIC_DRAW);
    
    glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT, m_UBO);
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    m_initialized = true;
    std::cout << "Transform UBO initialized successfully" << std::endl;
    
    return true;
}

void TransformUBO::UpdateTransform(const glm::mat4& modelMatrix) {
    if (!m_initialized) return;

    m_matrices.model = modelMatrix;
    m_matrices.normalMatrix = glm::transpose(glm::inverse(modelMatrix));

    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TransformMatrices), &m_matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TransformUBO::BindToPoint(unsigned int bindingPoint) {
    if (!m_initialized) return;
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_UBO);
}

void TransformUBO::Cleanup() {
    if (m_initialized && m_UBO != 0) {
        glDeleteBuffers(1, &m_UBO);
        m_UBO = 0;
        m_initialized = false;
    }
}
