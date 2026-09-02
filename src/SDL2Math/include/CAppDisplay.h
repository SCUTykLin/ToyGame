//
// Created by YangKai.Lin on 2026/9/2.
//

#ifndef TOYPROJECTS_CAPPDISPLAY_H
#define TOYPROJECTS_CAPPDISPLAY_H

#include <SDL2/SDL.h>

namespace Math::Display {
    class CAppDisplay {
    public:
        CAppDisplay();
        int OneExecute();
        bool OnInit();
        void OnEvent(SDL_Event &event);
        void OnLoop();
        void OnRender();
        void OnExit();
    private:
        bool isRunning;
        SDL_Window *pWindow;
        SDL_Renderer *pRenderer;
    };
}


#endif //TOYPROJECTS_CAPPDISPLAY_H
