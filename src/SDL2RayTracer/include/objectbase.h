//
// Created by YangKai.Lin on 2026/7/24.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_OBJECTBASE_H
#define TOYPROJECTS_SDL_RAYTRACING_OBJECTBASE_H

#include <glm/glm.hpp>
#include "ray.h"
#include "gtfm.h"

namespace RayTracing {
    class ObjectBase {
    public:
        ObjectBase();
        virtual ~ObjectBase();

        // Function to test for intersections.
        virtual bool TestIntersections(const Ray& castRay, glm::vec3& intPoint, glm::vec3& localNormal, glm::vec3& localColor);

        // Function to set the transform matrix.
        void SetTransformMatrix(const GTform & transformMatrix);

        // Function to test whether two floating-point numbers are close to being equal.
        bool CloseEnough(const double f1, const double f2);

    public:
        // The base color of the objects.
        glm::vec3 m_baseColor{255, 255, 255};

        // The geometric transform applied to the object.
        GTform m_transformMatrix;
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_OBJECTBASE_H