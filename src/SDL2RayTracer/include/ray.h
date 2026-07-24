//
// Created by YangKai.Lin on 2026/7/24.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_RAY_H
#define TOYPROJECTS_SDL_RAYTRACING_RAY_H

#include <glm/glm.hpp>

namespace RayTracing {
    class Ray {
    public:
        Ray();
        Ray(const glm::vec3 &point1, const glm::vec3 &point2);
        glm::vec3 GetPoint1() const;
        glm::vec3 GetPoint2() const;
        std::string toString() const;
    public:
        glm::vec3 m_point1{0, 0, 0};
        glm::vec3 m_point2{0, 0, 0};
        glm::vec3 m_lab{0, 0, 0};
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_RAY_H