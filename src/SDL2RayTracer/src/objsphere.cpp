//
// Created by YangKai.Lin on 2026/7/24.
//

#include "objsphere.h"

namespace RayTracing {
    Objsphere::Objsphere() {}

    Objsphere::~Objsphere() {}

    bool Objsphere::TestIntersections(const Ray &castRay, glm::vec3 &intPoint, glm::vec3 &localNormal,
        glm::vec3 &localColor) {
        glm::vec3 vhat = castRay.m_lab;
        vhat = glm::normalize(vhat);

        double b = 2.0 * glm::dot(castRay.m_point1, vhat);
        double c = glm::dot(castRay.m_point1, castRay.m_point1) - 1.0;

        double intTest = (b * b) - 4.0 * c;
        if (intTest > 0) {
            double numSQRT = sqrtf(intTest);
            double t1 = (-b + numSQRT) / 2.0;
            double t2 = (-b - numSQRT) / 2.0;

            if (t1 < 0 || t2 < 0){
                return false;
            } else{
                // Determine which point of intersection was closest to the camera.
                if (t1 < t2){
                    intPoint = castRay.m_point1 + vhat * static_cast<float>(t1);
                }else{
                    intPoint = castRay.m_point1 + vhat * static_cast<float>(t2);
                }
                return true;
            }
        }else {
            return false;
        }
    }
}
