//
// Created by YangKai.Lin on 2026/7/26.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_GRFM_H
#define TOYPROJECTS_SDL_RAYTRACING_GRFM_H

#include <glm/glm.hpp>
#include <glm/mat2x2.hpp>
#include "ray.h"

namespace RayTracing {
    constexpr bool FWDTFORM = true;
    constexpr bool BCKTFORM = false;

    class GTform {
    public:
        GTform();
        ~GTform();

        GTform(const glm::mat4x4& fwd, const glm::mat4x4& bck);

        void SetTransform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

        glm::mat4x4 GetForward();
        glm::mat4x4 GetBackward();

        Ray Apply(const Ray& inputRay, bool dirFlags);
        glm::vec3 Apply(const glm::vec3& inputVector, bool dirFlags);

        friend GTform operator*(const GTform& lfs, const GTform& rhs);
        GTform& operator=(const GTform& rhs);

        void PrintMatrix(bool dirFlag);
        static void PrintVector(const glm::vec3& vector);

    private:
        void Print(const glm::mat4x4& matrix);

    private:
        glm::mat4x4 m_fwdtfm{0};
        glm::mat4x4 m_bcktfm{0};
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_GRFM_H