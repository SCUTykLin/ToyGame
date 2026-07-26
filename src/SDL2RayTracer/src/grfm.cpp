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

    Ray GTform::Apply(const Ray &inputRay, bool dirFlags) {
    }

    GTform operator*(const GTform &lfs, const GTform &rhs) {
        GTform lfs1(lfs);
        return lfs1 *= rhs;
    }

    GTform & GTform::operator=(const GTform &rhs) {
    }

    void GTform::PrintMatrix(bool dirFlag) {
    }
}
