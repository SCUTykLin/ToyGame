//
// Created by YangKai.Lin on 2026/9/2.
//

#include "CAppDisplay.h"

#include <SDL.h>

namespace Math::Display {
    CAppDisplay::CAppDisplay() {
        isRunning = true;
        pWindow = nullptr;
        pRenderer = nullptr;
    }
    int CAppDisplay::OneExecute() {
    }
    bool CAppDisplay::OnInit() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return false;
        }
        
    }
    void CAppDisplay::OnEvent(SDL_Event &event) {
    }
    void CAppDisplay::OnLoop() {
    }
    void CAppDisplay::OnRender() {
    }
    void CAppDisplay::OnExit() {
    }
}
