//
// Created by YangKai.Lin on 2026/7/26.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_LIGHTBASE_H
#define TOYPROJECTS_SDL_RAYTRACING_LIGHTBASE_H

#include <memory>
#include <glm/glm.hpp>
#include "ray.h"
#include "objectbase.h"

namespace RayTracing {
    class Lightbase {
    public:
        Lightbase();
        virtual ~Lightbase();

        // Function to compute illumination contribution.
        virtual bool ComputeIllumination(const glm::vec3& intPoint, const glm::vec3& localNormal,
            const std::vector<std::shared_ptr<ObjectBase>>& objectList,
            const std::shared_ptr<ObjectBase>& currentObject,
            glm::vec3& color, double& intensity);
    public:
        glm::vec3 m_color{0.0f, 0.0f, 0.0f};
        glm::vec3 m_location{0.0f, 0.0f, 0.0f};
        double m_intensity{0.0f};
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_LIGHTBASE_H