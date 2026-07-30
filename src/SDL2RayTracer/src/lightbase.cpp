//
// Created by YangKai.Lin on 2026/7/26.
//

#include "lightbase.h"

namespace RayTracing {
    Lightbase::Lightbase() {
    }

    Lightbase::~Lightbase() {
    }

    bool Lightbase::ComputeIllumination(const glm::vec3 &intPoint,
        const glm::vec3 &localNormal,
        const std::vector<std::shared_ptr<ObjectBase>> &objectList,
        const std::shared_ptr<ObjectBase> &currentObject,
        glm::vec3 &color,
        double &intensity) {
        return false;
    }
}
