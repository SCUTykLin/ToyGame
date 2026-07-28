//
// Created by YangKai.Lin on 2026/7/25.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_MATH_H
#define TOYPROJECTS_SDL_RAYTRACING_MATH_H

#include <glm/glm.hpp>
#include <iostream>

namespace RayTracing {
    template <typename T>
    constexpr T PI = 3.14159265358979323846;

    constexpr float rad2deg(float rad) {
        return rad * 180.0f / PI<float>;
    }

    inline float computeFOV(float horzSize, float length) {
        auto rad = glm::atan(horzSize / length) * 2;
        return rad2deg(rad);
    }

    inline float computeFocus(float width, float fov) {
        return (width / 2.0f) / (fov / 2.0f);
    }

    inline void showMatrix(const glm::mat4x4 &m) {
        std::cout << "["
            << m[0][0] << "\t" << m[1][0] << "\t" << m[2][0] << "\t" << m[3][0] << "\n "
            << m[0][1] << "\t" << m[1][1] << "\t" << m[2][1] << "\t" << m[3][1] << "\n "
            << m[0][2] << "\t" << m[1][2] << "\t" << m[2][2] << "\t" << m[3][2] << "\n "
            << m[0][3] << "\t" << m[1][3] << "\t" << m[2][3] << "\t" << m[3][3] << "]" << std::endl;
    }

}

#endif //TOYPROJECTS_SDL_RAYTRACING_MATH_H