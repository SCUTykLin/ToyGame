//
// Created by YangKai.Lin on 2026/7/24.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_SCENE_H
#define TOYPROJECTS_SDL_RAYTRACING_SCENE_H

#include <vector>
#include <SDL2/SDL.h>
#include "qbImage.h"

namespace RayTracing {
    class Scene {
    public:
        Scene();

        // Function that perform rendering.
        bool Render(qbImage &outputImage);

    private:

    private:
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_SCENE_H