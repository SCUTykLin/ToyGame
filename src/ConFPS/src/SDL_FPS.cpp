//
// Created by 15813 on 2026/8/6.
//

#include "SDL_FPS.h"
#include <SDL_ttf.h>
#include <iostream>

namespace ConFPS {
    static SDL_Window *window = nullptr;
    static SDL_Surface *surface = nullptr;
    static SDL_Renderer *renderer = nullptr;
    static int nScreenWidth = 0;
    static int nScreenHeight = 0;

    static float* distance = nullptr;

    void FPS_Window::SetDistance(int x, int y, float value){
        distance[y * nScreenWidth + x] = value;
    }

    void FPS_Window::SetCategory(int x, int y, category_t cate){
        category[y * nScreenWidth + x] = cate;
    }

    void FPS_Window::init_window(int width, int height){
        nScreenWidth = width;
        nScreenHeight = height;
        distance = new float[nScreenWidth * nScreenHeight];
        category = new category_t[nScreenWidth * nScreenHeight];

        // TODO: 初始化distance为无穷，category为0.

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return;
        }

        if (TTF_Init() < 0) {
            fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
            return;
        }

        window = SDL_CreateWindow("FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

        if (window != nullptr) {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Set the background color to white
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
    }

    void render(wchar_t * screen){

    }
}
