//
// Created by YangKai.Lin on 2026/7/24.
//

#include "objectbase.h"
#define EPSILON 1e-11f
namespace RayTracing {
    ObjectBase::ObjectBase() {
    }

    ObjectBase::~ObjectBase() {
    }

    bool ObjectBase::TestIntersections(const Ray &castRay, glm::vec3 &intPoint, glm::vec3 &localNormal,
        glm::vec3 &localColor) {
        return false;
    }

    void ObjectBase::SetTransformMatrix(const GTform &transformMatrix) {
        m_transformMatrix = transformMatrix;
    }

    bool ObjectBase::CloseEnough(const double f1, const double f2) {
        return fabs(f1 - f2) < EPSILON;
    }
}
