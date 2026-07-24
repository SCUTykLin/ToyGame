//
// Created by 15813 on 2026/7/20.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_CAPP_H
#define TOYPROJECTS_SDL_RAYTRACING_CAPP_H

#define RacTracer_WIDTH 1280
#define RacTracer_HEIGHT 720

#include "SDL2/SDL.h"
#include "qbImage.h"
#include "scene.h"

namespace RayTracing {
    class CApp {
    public:
        CApp();
        int OnExecute();
        bool OnInit();
        void OnEvent(SDL_Event *event);
        void OnLoop();
        void OnRender();
        void OnExit();
    private:
        qbImage m_image;
        Scene m_scene;

        bool isRunning;
        SDL_Window* pWindow;
        SDL_Renderer* pRenderer;
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_CAPP_H
