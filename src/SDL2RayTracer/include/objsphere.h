//
// Created by YangKai.Lin on 2026/7/24.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_OBJSPHERE_H
#define TOYPROJECTS_SDL_RAYTRACING_OBJSPHERE_H

#include "objectbase.h"

namespace RayTracing {
    class Objsphere : public ObjectBase {
    public:
        Objsphere();
        virtual ~Objsphere() override;
        virtual bool TestIntersections(const Ray &castRay, glm::vec3 &intPoint, glm::vec3 &localNormal, glm::vec3 &localColor) override;
    private:

    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_OBJSPHERE_H