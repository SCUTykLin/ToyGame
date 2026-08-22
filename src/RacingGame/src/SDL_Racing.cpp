//
// Created by 15813 on 2026/8/19.
//

#include "SDL_Racing.h"
#include <iostream>

namespace Racing{

    static SDL_Window* window = nullptr;
    static SDL_Renderer* renderer = nullptr;
    static SDL_Texture* framebufferTexture = nullptr;
    static int ScreenWidth = 0;
    static int ScreenHeight = 0;
    static int RenderWidth = 160;
    static int RenderHeight = 100;
    static bool isRunning = false;

    void SDL_gameLoop(int Width, int Height){
        ScreenWidth = Width;
        ScreenHeight = Height;

        gameData_t gameData;
        gameCreate(gameData);

        while (isRunning){
            gameEvent(gameData);
            gameUpdate(gameData);
            gameRender(gameData);
        }
    }

    void gameCreate(gameData_t& gameData){
        if (SDL_Init(SDL_INIT_VIDEO) < 0){
            std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
            return;
        }
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        window = SDL_CreateWindow("Racing Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
        if (!window){
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
            return;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer){
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
            return;
        }

        framebufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, RenderWidth, RenderHeight);
        if (!framebufferTexture){
            std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << '\n';
            return;
        }

        gameData.DataInit();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    void gameEvent(gameData_t& gameData){

        gameData.evener->EventInit();

        SDL_Event event;
        if (event.type == SDL_MOUSEWHEEL)
        {
            if (event.wheel.y > 0){
                gameData.evener->wheel = 1;
            }
            else if (event.wheel.y < 0){
                gameData.evener->wheel = -1;
            }
        }

        if (event.key.keysym.sym == SDLK_UP){
            gameData.evener->keys[0] = true;
        }

        if (event.key.keysym.sym == SDLK_LEFT){
            gameData.evener->keys[1] = true;
        }

        if (event.key.keysym.sym == SDLK_RIGHT){
            gameData.evener->keys[2] = true;
        }

    }

    void gameInit(gameData_t& gameData){

    }

    void gameUpdate(gameData_t& gameData){

    }

    void gameRender(gameData_t& gameData){

    }

    void gameText(gameData_t& gameData){

    }

    carGraph::carGraph() {

    }

    std::array<SDL_Rect, 10> &carGraph::getCarGraph(carGraph::CarStatus s) {
        switch (s) {
            case NORMAL: return normal_graph;
                break;
            case RIGHT: return right_graph;
                break;
            case LEFT: return left_graph;
                break;
            default:
                break;
        }
    }

    void gameDestroy(gameData_t& gameData){

    }
}