//
// Created by YangKai.Lin on 2026/7/26.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_POINTLIGHT_H
#define TOYPROJECTS_SDL_RAYTRACING_POINTLIGHT_H
#include "lightbase.h"

namespace RayTracing {
    class Pointlight : public Lightbase{
    public:
        Pointlight();
        virtual ~Pointlight() override;

        // Function to compute illumination
        virtual bool ComputeIllumination(const glm::vec3 &intPoint, const glm::vec3 &localNormal, const std::vector<std::shared_ptr<ObjectBase>> &objectList, const std::shared_ptr<ObjectBase> &currentObject, glm::vec3 &color, double &intensity) override;
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_POINTLIGHT_H