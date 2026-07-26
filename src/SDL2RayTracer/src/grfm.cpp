//
// Created by YangKai.Lin on 2026/7/26.
//

#include "grfm.h"

#include <glm/mat4x4.hpp>

namespace RayTracing {
    GTform::GTform() {
        m_fwdtfm = glm::mat4x4(1.0f);
        m_bcktfm = glm::mat4x4(1.0f);
    }

    GTform::~GTform() {
    }

    GTform::GTform(const glm::mat4x4 &fwd, const glm::mat4x4 &bck) {
        // Verify that the inputs are 4x4.
        m_fwdtfm = fwd;
        m_bcktfm = bck;
    }

    void GTform::SetTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale) {
    }

    glm::mat4x4 GTform::GetForward() {
    }

    glm::mat4x4 GTform::GetBackward() {
    }

    glm::vec3 GTform::Apply(const glm::vec3 &inputVector, bool dirFlags) {
    }

    void GTform::PrintVector(const glm::vec3 &vector) {
    }

    void GTform::Print(const glm::mat4x4 &matrix) {
    }

    Ray GTform::Apply(const Ray &inputRay, bool dirFlags) {
    }

    GTform operator*(const GTform &lfs, const GTform &rhs) {
    }

    GTform & GTform::operator=(const GTform &rhs) {
    }

    void GTform::PrintMatrix(bool dirFlag) {
    }
}
