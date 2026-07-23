//
// Created by YangKai.Lin on 2026/7/19.
//

#include "CApp.h"

namespace RayTracing {
    CApp::CApp() {
        isRunning = true;
        pWindow = nullptr;
        pRenderer = nullptr;
    }

    int CApp::OnExecute() {
    }

    bool CApp::OnInit() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return false;
        }
    }

    void CApp::OnEvent(SDL_Event *event) {
    }

    void CApp::OnLoop() {
    }

    void CApp::OnRender() {
    }

    void CApp::OnExit() {
    }
}
