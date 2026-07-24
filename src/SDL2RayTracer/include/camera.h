//
// Created by YangKai.Lin on 2026/7/24.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_CAMERA_H
#define TOYPROJECTS_SDL_RAYTRACING_CAMERA_H

#include <glm/glm.hpp>
#include "ray.h"

namespace RayTracing {
    class Camera {
    public:
        Camera();

        // Functions to set camera parameters.
        void SetPosition(const glm::vec3& newPosition);
        void SetLookAt(const glm::vec3& newLookAt);
        void SetUp(const glm::vec3& newUpVector);
        void SetLength(double newLength);
        void SetHorzSize(double newSize);
        void SetAspect(double newAspect);

        // Functions to return camera parameters
        glm::vec3 GetPosition() const;
        glm::vec3 GetLookAt() const;
        glm::vec3 GetUp() const;
        glm::vec3 GetU() const;
        glm::vec3 GetV() const;
        glm::vec3 GetScreenCenter() const;
        double GetLength() const;
        double GetHorzSize() const;
        double GetAspect() const;

        // Function to generate a ray
        bool GenerateRay(float proScreenX, float proScreenY, Ray& cameraRay);

        // Update Camera Geometry
        void UpdateCameraGeometry();

        std::string toString() const;
    private:
        glm::vec3 m_cameraPosition{0, 0, 0};
        glm::vec3 m_cameraLookAt{0, 0, 0};
        glm::vec3 m_cameraUp{0, 1, 0};
        double m_cameraLength{0};
        double m_cameraHorzSize{0};
        double m_cameraAspect{0};

        glm::vec3 m_alignmentVector{0, 0, 0};
        glm::vec3 m_projectionScreenU{0, 0, 0};
        glm::vec3 m_projectionScreenV{0, 0, 0};
        glm::vec3 m_projectionScreenCenter{0, 0, 0};
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_CAMERA_H