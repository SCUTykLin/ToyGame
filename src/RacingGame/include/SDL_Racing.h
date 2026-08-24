//
// Created by 15813 on 2026/8/19.
//

#ifndef TOYPROJECTS_RACE_SDL_RACING_H
#define TOYPROJECTS_RACE_SDL_RACING_H

#include <SDL2/SDL.h>
#include <array>
#include <vector>
#include <list>
#include "car.h"
#include <chrono>
#include "env.h"

namespace Racing{

    typedef struct gamePlayer{
        float x = 0, y = 0;
        float cell = 1.0f;

        float fCarPos = 0.0f;
        float fDistance = 0.0f;
        float fCurvature = 0.0f;
        float fSpeed = 0.0f;

        float fTrackCurvature = 0.0f;
        float fPlayerCurvature = 0.0f;
        float fTrackDistance = 0.0f;
        float fCurrentLapTime = 0.0f;

        std::list<float> listLapTimes;
        std::vector<std::pair<float, float>> vecTrack;
        carGraph_t::CarStatus car_status;

        void PlayerInit(){
            x = 0.5f, y = 0.75f;
            cell = 2.0f;
            car_status = carGraph::NORMAL;
        }
    }gamePlayer_t;

    typedef struct gameScore{
        void ScoreInit(){

        }
    }gameScore_t;

    typedef struct gameTime{
        using Clock = std::chrono::steady_clock;
        Clock::time_point previousTime;
        float deltaTime = 0.0f;

        void Start(){
            previousTime = Clock::now();
            deltaTime = 0.0f;
        }

        void Tick() {
            const auto currentTime = Clock::now();
            deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
            previousTime = currentTime;
        }
    }gameTime_t;

    typedef struct gameData{
        gamePlayer_t* player;
        gameScore_t* scorer;
        gameTime_t* timer;
        carGraph_t* car;
        Env_t* env;

        void DataInit(){
            player = new gamePlayer_t();
            scorer = new gameScore_t();
            timer = new gameTime_t();
            car = new carGraph_t();
            env = new Env_t();

            player->PlayerInit();
            scorer->ScoreInit();
            timer->Start();
        }
    }gameData_t;

    void SDL_gameLoop(int ScreenWidth, int ScreenHeight);
    void gameCreate(gameData_t& gameData);
    void gameEvent(gameData_t& gameData);
    void gameInit(gameData_t& gameData);
    void gameUpdate(gameData_t& gameData);
    void gameRender(gameData_t& gameData);
    void gameText(gameData_t& gameData);
    void gameDestroy(gameData_t& gameData);
}

#endif //TOYPROJECTS_RACE_SDL_RACING_H
