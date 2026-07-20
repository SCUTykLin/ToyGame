//
// Created by 15813 on 2026/7/20.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_CAPP_H
#define TOYPROJECTS_SDL_RAYTRACING_CAPP_H

#include "SDL2/SDL.h"

namespace RayTracing {
    class CApp {
    public:
        int OnExecute();
        bool OnInit();
        void OnEvent(SDL_Event *event);
        void OnLoop();
        void OnRender();
        void OnExit();
    private:
        bool isRunning;
        SDL_Window* pWindow;
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_CAPP_H
