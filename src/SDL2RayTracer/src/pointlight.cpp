//
// Created by YangKai.Lin on 2026/7/26.
//

#include "pointlight.h"

namespace RayTracing {
    Pointlight::Pointlight() {
        m_color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_intensity = 1.0;
    }

    Pointlight::~Pointlight() {
    }

    bool Pointlight::ComputeIllumination(const glm::vec3 &intPoint, const glm::vec3 &localNormal,
        const std::vector<std::shared_ptr<ObjectBase>> &objectList, const std::shared_ptr<ObjectBase> &currentObject,
        glm::vec3 &color, double &intensity) {
        glm::vec3 lighDir = glm::normalize(m_location - intPoint); // Construct a vector pointing from the intersection point to the light.
        auto startPoint = intPoint; // Compute a Start Point.
        float angle = glm::acos(glm::dot(localNormal, lighDir));

        if (angle > 1.5708) {
            color = m_color;
            intensity = 0.0;
            return false;
        }else {
            color = m_color; // do have illumination.
            intensity = m_intensity * (1.0 - (angle / 1.5708));
            return true;
        }
    }
}
