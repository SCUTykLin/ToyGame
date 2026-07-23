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
        SDL_Event event;
        if (!OnInit()) {
            return -1;
        }
        while (isRunning) {
            while (SDL_PollEvent(&event)) {
                OnEvent(&event);
            }
            OnLoop();
            OnRender();
        }
        OnExit();
    }

    bool CApp::OnInit() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return false;
        }
        pWindow = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RacTracer_WIDTH, RacTracer_HEIGHT, SDL_WINDOW_SHOWN);
        if (pWindow != nullptr) {
            pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
            m_image.Initialize(RacTracer_WIDTH, RacTracer_HEIGHT, pRenderer);

            for (int x = 0; x < RacTracer_WIDTH; ++x)
                for (int y = 0; y < RacTracer_HEIGHT; ++y) {
                    double red = (static_cast<double>(x) / 1280.0) * 255.0;
                    double green = (static_cast<double>(y) / 1280.0) * 255.0;
                    m_image.SetPixel(x, y, red, green, 0.0);
                }
        }else {
            return false;
        }
        return true;
     }
    void CApp::OnEvent(SDL_Event *event) {
        if (event->type == SDL_QUIT) {
            isRunning = false;
        }
    }

    void CApp::OnLoop() {

    }

    void CApp::OnRender() {
        // Set  background
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pRenderer);

        // display image
        m_image.Display();

        // show result
        SDL_RenderPresent(pRenderer);
    }

    void CApp::OnExit() {
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        pWindow = nullptr;
        pRenderer = nullptr;
        SDL_Quit();
    }
}
