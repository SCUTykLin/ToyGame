//
// Created by YangKai.Lin on 2026/9/2.
//

#include "CAppDisplay.h"

namespace Math::Display {
    CAppDisplay::CAppDisplay() {
        isRunning = true;
        pWindow = nullptr;
        pRenderer = nullptr;

        m_redList.push_back(51); m_greenList.push_back(128); m_blueList.push_back(204);
        m_redList.push_back(255); m_greenList.push_back(128); m_blueList.push_back(0);
        m_redList.push_back(255); m_greenList.push_back(191); m_blueList.push_back(0);
    }
    int CAppDisplay::OneExecute() {
        SDL_Event event;
        if (!OnInit()) {
            return -1;
        }
        while (isRunning) {
            while(SDL_PollEvent(&event)) {
                OnEvent(&event);
            }
            OnLoop();
            OnRender();
        }
        OnExit();
        return 0;
    }
    bool CAppDisplay::OnInit() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return false;
        }
        pWindow = SDL_CreateWindow("SDL3 Display  Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);

        if (pWindow != nullptr) {
            pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_PRESENTVSYNC);
        }else {
            return false;
        }

        m_turtle.MoveTo(320, 240);

        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pRenderer);

        m_turtle.SetRenderer(pRenderer);
        m_turtle.PenDown();
        m_turtle.SetLength(75);

        for (int i = 0; i < 18; ++i) {
            m_turtle.SetPenColor(m_redList[i % 3], m_greenList[i % 3], m_blueList[i % 3], 255);
            for (int j = 0; j < 8; j++) {
                m_turtle.RotateRight(45);
                m_turtle.Step();
            }
            m_turtle.RotateLeft(20);
        }

        SDL_RenderPresent(pRenderer);

        return true;
    }
    void CAppDisplay::OnEvent(SDL_Event *event) {
        if (event->type == SDL_QUIT) {
            isRunning = false;
        }
    }
    void CAppDisplay::OnLoop() {
    }
    void CAppDisplay::OnRender() {
        // SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        // SDL_RenderClear(pRenderer);
        //
        // SDL_Rect fillRect = {150, 120, 300, 200};
        // SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
        // SDL_RenderFillRect(pRenderer, &fillRect);
        // SDL_RenderPresent(pRenderer);
    }
    void CAppDisplay::OnExit() {
        SDL_DestroyWindow(pWindow);
        pWindow = nullptr;
        SDL_DestroyRenderer(pRenderer);
        pRenderer = nullptr;
        SDL_Quit();
    }
}
