//
// Created by YangKai.Lin on 2026/7/24.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_SCENE_H
#define TOYPROJECTS_SDL_RAYTRACING_SCENE_H

#include <vector>
#include <SDL2/SDL.h>
#include "qbImage.h"
#include "camera.h"
#include "objsphere.h"

namespace RayTracing {
    class Scene {
    public:
        Scene(float fov, float width, float height);

        // Function that perform rendering.
        bool Render(qbImage &outputImage);
        void SetFOV(float fov);
        float GetFOV();
        float GetFOVRad();
        void SetWidth(float Width);
        float GetWidth();
        void SetHeight(float Height);
        float GetHeight();
    private:

    private:
        float fov;
        float width;
        float height;
        Camera m_camera;
        Objsphere m_testSphere;
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_SCENE_H