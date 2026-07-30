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

        // Construct a ray from the point of intersection to the light.
        Ray lightRay{startPoint, startPoint + lighDir};

        // Check for intersection with all of the objects. except for the current one.
        glm::vec3 poi{0.0, 0.0, 0.0};
        glm::vec3 poiNormal{0.0, 0.0, 0.0};
        glm::vec3 poiColor{0.0, 0.0, 0.0};
        bool validInt = false;
        for (auto sceneObject : objectList){
            if (sceneObject != currentObject){
                validInt = sceneObject->TestIntersections(lightRay, poi, poiNormal, poiColor);
            }
            // If have intersection, no point check further.
            if (validInt) break;
        }

        if (!validInt){
            // Compute the angle between the curr
        }

        return true;
    }
}
