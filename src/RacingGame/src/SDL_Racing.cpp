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
    static int RenderWidth = 720;
    static int RenderHeight = 540;
    static bool isRunning = false;

    void SDL_gameLoop(int Width, int Height){
        ScreenWidth = Width;
        ScreenHeight = Height;

        gameData_t gameData;
        gameInit(gameData);
        gameCreate(gameData);

        while (isRunning){
            gameData.timer->Tick();
            gameEvent(gameData);
            gameUpdate(gameData);
            gameRender(gameData);
        }
        gameDestroy(gameData);
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

        framebufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, RenderWidth, RenderHeight);
        if (!framebufferTexture){
            std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << '\n';
            return;
        }
        isRunning = true;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    void gameEvent(gameData_t& gameData){
        SDL_Event event;

        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                isRunning = false;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
                isRunning = false;
            }
        }
    }

    void gameInit(gameData_t& gameData){
        gameData.DataInit();
        gameData.env->scaleEnv(RenderWidth, RenderHeight);

        gameData.player->vecTrack.push_back(std::make_pair(0.0f, 10.0f));
        gameData.player->vecTrack.push_back(std::make_pair(0.0f, 200.0f));
        gameData.player->vecTrack.push_back(std::make_pair(1.0f, 200.0f));
        gameData.player->vecTrack.push_back(std::make_pair(0.0f, 400.0f));
        gameData.player->vecTrack.push_back(std::make_pair(-1.0f, 100.0f));
        gameData.player->vecTrack.push_back(std::make_pair(0.0f, 200.0f));
        gameData.player->vecTrack.push_back(std::make_pair(-1.0f, 200.0f));
        gameData.player->vecTrack.push_back(std::make_pair(1.0f, 200.0f));
        gameData.player->vecTrack.push_back(std::make_pair(0.0f, 200.0f));
        gameData.player->vecTrack.push_back(std::make_pair(0.2f, 500.0f));
        gameData.player->vecTrack.push_back(std::make_pair(0.0f, 200.0f));

        for (auto t : gameData.player->vecTrack)
            gameData.player->fTrackDistance += t.second;

        gameData.player->listLapTimes = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    }

    void gameUpdate(gameData_t& gameData){
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_UP]){
            gameData.player->fSpeed += 2.0f * gameData.timer->deltaTime;
        }else{
            gameData.player->fSpeed -= 1.0f * gameData.timer->deltaTime;
        }

        int carDirection = 0;
        if (keys[SDL_SCANCODE_LEFT]){
            gameData.player->fPlayerCurvature -= 0.7f * gameData.timer->deltaTime;
            carDirection = -1;
        }

        if (keys[SDL_SCANCODE_RIGHT]){
            gameData.player->fPlayerCurvature += 0.7f * gameData.timer->deltaTime;
            carDirection = 1;
        }

        if (fabs(gameData.player->fPlayerCurvature - gameData.player->fTrackCurvature) > 0.8f){
            gameData.player->fSpeed -= 5.0f * gameData.timer->deltaTime;
        }

        gameData.player->fSpeed = std::clamp(gameData.player->fSpeed, 0.0f, 1.0f);
        gameData.player->fDistance += (70.f * gameData.player->fSpeed) * gameData.timer->deltaTime;

        float fOffset = 0;
        int nTrackSection = 0;

        gameData.player->fCurrentLapTime += gameData.timer->deltaTime;
        if (gameData.player->fDistance >= gameData.player->fTrackDistance){
            gameData.player->fDistance -= gameData.player->fTrackDistance;
            gameData.player->listLapTimes.push_front(gameData.player->fCurrentLapTime);
            gameData.player->fCurrentLapTime = 0.0f;
        }

        while(nTrackSection < gameData.player->vecTrack.size() && fOffset <= gameData.player->fDistance){
            fOffset += gameData.player->vecTrack[nTrackSection].second;
            nTrackSection++;
        }

        float fTargetCurvature = gameData.player->vecTrack[nTrackSection - 1].first;
        float fTrackCurveDiff = (fTargetCurvature - gameData.player->fCurvature) * gameData.timer->deltaTime * gameData.player->fSpeed;
        gameData.player->fCurvature += fTrackCurveDiff;

        gameData.player->fTrackCurvature += (gameData.player->fCurvature) * gameData.timer->deltaTime * gameData.player->fSpeed;

        gameData.env->GenerateScaleMount(RenderWidth, RenderHeight, gameData.player->fTrackCurvature);

        gameData.env->GenerateScaleGrass(RenderWidth, RenderHeight, gameData.player->fCurvature, gameData.player->fDistance, nTrackSection);

        gameData.env->GenerateRoadMark(RenderWidth, RenderHeight, gameData.player->fCurvature, gameData.player->fDistance, (70.f * gameData.player->fSpeed) * gameData.timer->deltaTime);

        gameData.player->fCarPos = gameData.player->fPlayerCurvature - gameData.player->fTrackCurvature;
        int nCarPos = RenderWidth / 2 + ((int)(RenderWidth * gameData.player->fCarPos) / 2.0f) - 7;

        switch (carDirection){
            case 0:
                gameData.player->car_status = carGraph::NORMAL;
                break;
            case 1:
                gameData.player->car_status = carGraph::RIGHT;
                break;
            case -1:
                gameData.player->car_status = carGraph::LEFT;
                break;
        }
        gameData.player->x = nCarPos;
        gameData.player->y = 400;
    }

    void gameRender(gameData_t& gameData){
        SDL_SetRenderTarget(renderer, framebufferTexture);
        SDL_RenderClear(renderer);

        gameData.env->renderEnv(renderer);
        gameData.car->render(gameData.player->x, gameData.player->y,
                             gameData.player->cell, gameData.player->car_status, renderer);

        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Rect destRect = {0, 0, ScreenWidth, ScreenHeight};
        SDL_RenderCopy(renderer, framebufferTexture, nullptr, &destRect);

        SDL_RenderPresent(renderer);
    }

    void gameText(gameData_t& gameData){

    }

    void gameDestroy(gameData_t& gameData){
        if (framebufferTexture){
            SDL_DestroyTexture(framebufferTexture);
            framebufferTexture = nullptr;
        }

        if (renderer){
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window){
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }
}