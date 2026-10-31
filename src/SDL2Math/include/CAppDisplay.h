//
// Created by YangKai.Lin on 2026/9/2.
// Fractals of SDL2
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_CAPPDISPLAY_H
#define TOYPROJECTS_SDL_RAYTRACING_CAPPDISPLAY_H

#include <array>
#include <SDL2/SDL.h>
#include "Turtle.h"
#include "DragonCurve.h"
#include "qbvkGenerator.h"
#include "qbvkInitiator.h"

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

        DragonCurve m_dragon;

        qbvkGenerator m_vkGenerator;
        qbvkInitiator m_vkInitiator;

        int m_level;

        void DrawEdge(float x1, float y1, float x2, float y2, int level);

        void DrawDragon();

        void SetHueColor(float hue);

        void Redraw();

        std::array<int, 3> m_redList{51, 255, 255};
        std::array<int, 3> m_greenList{128, 218, 191};
        std::array<int, 3> m_blueList{204, 0, 0};
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_CAPPDISPLAY_H
