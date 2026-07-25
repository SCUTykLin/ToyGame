//
// Created by YangKai.Lin on 2026/7/25.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_MATH_H
#define TOYPROJECTS_SDL_RAYTRACING_MATH_H

#include <glm/glm.hpp>

namespace RayTracing {
    template <typename T>
    constexpr T PI = 3.14159265358979323846;

    constexpr float rad2deg(float rad) {
        return rad * 180.0f / PI<float>;
    }

    float computeFOV(float horzSize, float length) {
        auto rad = glm::atan(horzSize / length) * 2;
        return rad2deg(rad);
    }

    float computeFocus(float width, float fov) {
        return (width / 2.0f) / (fov / 2.0f);
    }

}

#endif //TOYPROJECTS_SDL_RAYTRACING_MATH_H