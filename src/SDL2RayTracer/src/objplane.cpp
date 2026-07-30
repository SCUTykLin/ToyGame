
//
// Created by YangKai.Lin on 2026/7/29.
//

#include "objplane.h"
#include <cmath>

namespace RayTracing {
    ObjPlane::ObjPlane() {
    }

    ObjPlane::~ObjPlane() {
    }

    bool ObjPlane::TestIntersections(const Ray &castRay, glm::vec3 &intPoint, glm::vec3 &localNormal,
        glm::vec3 &localColor) {

        // Copy the ray and apply the backwards transform.
        Ray bckRay = m_transformMatrix.Apply(castRay, BCKTFORM);

        // Copy the m_lab vector from bckRay and normalize it.
        glm::vec3 k = bckRay.m_lab;
        k = glm::normalize(k);

        // Check if there is an intersection.
        if (!CloseEnough(k.z, 0.0f)) {
            // There is an intersection.
            float t = bckRay.m_point1.z / -k.z;

            // If t is negative, then the intersection point must be behind the camera and we can ignore it.
            if (t > 0.0f) {
                // Compute the values for u and v.
                float u = bckRay.m_point1.x + k.x * t;
                float v = bckRay.m_point1.y + k.y * t;

                if (abs(u) < 1.0 && abs(v) < 1.0) {
                    glm::vec3 poi = bckRay.m_point1 + t * k;

                    // Transform the intersection point back into world coordinate.
                    intPoint = m_transformMatrix.Apply(poi, FWDTFORM);

                    // Compute the local normal.
                    glm::vec3 localOrigin{0, 0, 0};
                    glm::vec3 normalVector{0, 0, -1};
                    glm::vec3 globalOrigin = m_transformMatrix.Apply(localOrigin, FWDTFORM);
                    localNormal = m_transformMatrix.Apply(normalVector, FWDTFORM) - globalOrigin;
                    localNormal = glm::normalize(localNormal);

                    // Return the base color;
                    localColor = m_baseColor;

                    return true;
                }
                return false;
            }
            return false;
        }
        return false;
    }
}
