//
// Created by YangKai.Lin on 2026/9/2.
//

#include "CAppDisplay.h"
#include <algorithm>
#include <vector>

namespace Math::Display {
    CAppDisplay::CAppDisplay() {
        isRunning = true;
        pWindow = nullptr;
        pRenderer = nullptr;
        m_level = 14;
    }

    int CAppDisplay::OneExecute() {
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
        return 0;
    }

    void CAppDisplay::DrawEdge(float x1, float y1, float x2, float y2, int level) {
        level--;
        std::vector<float> edgeX1;
        std::vector<float> edgeY1;
        std::vector<float> edgeX2;
        std::vector<float> edgeY2;

        float gx1, gy1, gx2, gy2;
        m_vkGenerator.SetStartPoint(x1, y1);
        m_vkGenerator.SetEndPoint(x2, y2);
        m_vkGenerator.Compute();

        for (int j = 0; j < m_vkGenerator.NumEdges(); ++j) {
            if (m_vkGenerator.GetEdge(&gx1, &gy1, &gx2, &gy2, j)) {
                edgeX1.push_back(gx1);
                edgeY1.push_back(gy1);
                edgeX2.push_back(gx2);
                edgeY2.push_back(gy2);
            }
        }

        for (int j = 0; j < m_vkGenerator.NumEdges(); ++j) {
            if (level == 0) {
                SDL_RenderDrawLine(pRenderer,
                                   static_cast<int>(round(edgeX1[j])),
                                   static_cast<int>(round(edgeY1[j])),
                                   static_cast<int>(round(edgeX2[j])),
                                   static_cast<int>(round(edgeY2[j])));
            }else {
                DrawEdge(edgeX1[j], edgeY1[j], edgeX2[j], edgeY2[j], level);
            }
        }
    }

    void CAppDisplay::DrawDragon() {
        m_dragon.SetStartPoint(0.0f, 0.0f);
        m_dragon.SetLength(1.0f);
        m_dragon.SetLevel(m_level);
        m_dragon.Compute();

        float x1, y1, x2, y2;
        float minX = 0.0f, minY = 0.0f;
        float maxX = 0.0f, maxY = 0.0f;
        for (int i = 0; i < m_dragon.NumEdges(); ++i) {
            if (m_dragon.GetEdge(&x1, &y1, &x2, &y2, i)) {
                minX = std::min(minX, std::min(x1, x2));
                minY = std::min(minY, std::min(y1, y2));
                maxX = std::max(maxX, std::max(x1, x2));
                maxY = std::max(maxY, std::max(y1, y2));
            }
        }

        int width, height;
        SDL_GetRendererOutputSize(pRenderer, &width, &height);

        float margin = 20.0f;
        float spanX = maxX - minX;
        float spanY = maxY - minY;
        float length = std::min((static_cast<float>(width) - margin * 2.0f) / spanX,
                                (static_cast<float>(height) - margin * 2.0f) / spanY);
        float startX = (static_cast<float>(width) - spanX * length) * 0.5f - minX * length;
        float startY = (static_cast<float>(height) - spanY * length) * 0.5f - minY * length;

        m_dragon.SetStartPoint(startX, startY);
        m_dragon.SetLength(length);
        m_dragon.Compute();

        float total = static_cast<float>(m_dragon.NumEdges());
        for (int i = 0; i < m_dragon.NumEdges(); ++i) {
            if (m_dragon.GetEdge(&x1, &y1, &x2, &y2, i)) {
                SetHueColor(static_cast<float>(i) / total);
                SDL_RenderDrawLine(pRenderer,
                                   static_cast<int>(round(x1)), static_cast<int>(round(y1)),
                                   static_cast<int>(round(x2)), static_cast<int>(round(y2)));
            }
        }
    }

    void CAppDisplay::SetHueColor(float hue) {
        float h = hue * 6.0f;
        int sector = static_cast<int>(h);
        float f = h - static_cast<float>(sector);
        float p = 0.0f;
        float q = 1.0f - f;
        float t = f;

        float r, g, b;
        switch (sector % 6) {
            case 0:
                r = 1.0f;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = 1.0f;
                b = p;
                break;
            case 2:
                r = p;
                g = 1.0f;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = 1.0f;
                break;
            case 4:
                r = t;
                g = p;
                b = 1.0f;
                break;
            default:
                r = 1.0f;
                g = p;
                b = q;
                break;
        }

        SDL_SetRenderDrawColor(pRenderer,
                               static_cast<Uint8>(r * 255.0f),
                               static_cast<Uint8>(g * 255.0f),
                               static_cast<Uint8>(b * 255.0f),
                               255);
    }

    void CAppDisplay::Redraw() {
        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pRenderer);

        DrawDragon();

        SDL_RenderPresent(pRenderer);
    }

    bool CAppDisplay::OnInit() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return false;
        }
        pWindow = SDL_CreateWindow("SDL3 Display  Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480,
                                   SDL_WINDOW_SHOWN);

        if (pWindow != nullptr) {
            pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_PRESENTVSYNC);
        } else {
            return false;
        }

        Redraw();

        return true;
    }

    void CAppDisplay::OnEvent(SDL_Event *event) {
        if (event->type == SDL_QUIT) {
            isRunning = false;
        }
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_EQUALS || event->key.keysym.sym == SDLK_PLUS) {
                m_level = std::min(m_level + 1, 18);
                Redraw();
            } else if (event->key.keysym.sym == SDLK_MINUS) {
                m_level = std::max(m_level - 1, 1);
                Redraw();
            }
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
