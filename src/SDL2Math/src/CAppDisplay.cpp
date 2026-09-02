//
// Created by YangKai.Lin on 2026/9/2.
//

#include "CAppDisplay.h"

namespace Math::Display {
    CAppDisplay::CAppDisplay() {
        isRunning = true;
        pWindow = nullptr;
        pRenderer = nullptr;
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

        return true;
    }
    void CAppDisplay::OnEvent(SDL_Event *event) {
    }
    void CAppDisplay::OnLoop() {
    }
    void CAppDisplay::OnRender() {
    }
    void CAppDisplay::OnExit() {
    }
}
