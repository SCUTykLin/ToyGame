//
// Created by YangKai.Lin on 2026/9/2.
// Fractals of SDL2
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_CAPPDISPLAY_H
#define TOYPROJECTS_SDL_RAYTRACING_CAPPDISPLAY_H

#include <vector>
#include <SDL2/SDL.h>
#include "Turtle.h"

namespace Math::Display {
    class CAppDisplay {
    public:
        CAppDisplay();
        int OneExecute();
        bool OnInit();
        void OnEvent(SDL_Event *event);
        void OnLoop();
        void OnRender();
        void OnExit();
    private:
        bool isRunning;
        SDL_Window *pWindow;
        SDL_Renderer *pRenderer;

        Turtle m_turtle;
        std::vector<int> m_redList;
        std::vector<int> m_greenList;
        std::vector<int> m_blueList;
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_CAPPDISPLAY_H
