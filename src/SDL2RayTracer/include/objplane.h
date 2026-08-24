//
// Created by YangKai.Lin on 2026/7/29.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_OBJPLANE_H
#define TOYPROJECTS_SDL_RAYTRACING_OBJPLANE_H

#include "objectbase.h"
#include "gtfm.h"
#include <glm/glm.hpp>

namespace RayTracing {
    class ObjPlane : public ObjectBase {
    public:
        ObjPlane();

        virtual ~ObjPlane() override;

        virtual bool TestIntersections(const Ray &castRay, glm::vec3 &intPoint, glm::vec3 &localNormal, glm::vec3 &localColor) override;

    private:

    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_OBJPLANE_H