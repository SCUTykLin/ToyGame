//
// Created by YangKai.Lin on 2026/7/24.
//

#include "objsphere.h"

namespace RayTracing {
    Objsphere::Objsphere() {}

    Objsphere::~Objsphere() {}

    bool Objsphere::TestIntersections(const Ray &castRay, glm::vec3 &intPoint, glm::vec3 &localNormal,
        glm::vec3 &localColor) {

        // Copy the ray and apply the backwards transform. (world -> local)
        Ray bckRay = m_transformMatrix.Apply(castRay, BCKTFORM);

        glm::vec3 vhat = bckRay.m_lab;
        vhat = glm::normalize(vhat);

        double b = 2.0 * glm::dot(bckRay.m_point1, vhat);
        double c = glm::dot(bckRay.m_point1, bckRay.m_point1) - 1.0;

        double intTest = (b * b) - 4.0 * c;

        glm::vec3 poi;
        if (intTest > 0) {
            double numSQRT = sqrtf(intTest);
            double t1 = (-b + numSQRT) / 2.0;
            double t2 = (-b - numSQRT) / 2.0;

            if (t1 < 0 || t2 < 0){
                return false;
            } else{
                // Determine which point of intersection was closest to the camera.
                if (t1 < t2){
                    poi = bckRay.m_point1 + vhat * static_cast<float>(t1);
                }else{
                    poi = bckRay.m_point1 + vhat * static_cast<float>(t2);
                }

                // Transform the intersection point back into world coodinates.
                intPoint = m_transformMatrix.Apply(poi, FWDTFORM);
                // Compute Local normal
                glm::vec3 objOrigin{0.0, 0.0, 0.0};
                glm::vec3 newObjOrigin = m_transformMatrix.Apply(objOrigin, FWDTFORM);
                localNormal = intPoint - newObjOrigin;
                localNormal = glm::normalize(localNormal);

                // Return the base color.
                localColor = m_baseColor;

                return true;
            }
        }else {
            return false;
        }
    }
}
