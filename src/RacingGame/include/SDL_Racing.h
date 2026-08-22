//
// Created by 15813 on 2026/8/19.
//

#ifndef TOYPROJECTS_RACE_SDL_RACING_H
#define TOYPROJECTS_RACE_SDL_RACING_H

#include <SDL2/SDL.h>
#include <array>

namespace Racing{

    typedef struct gamePlayer{
        void PlayerInit(){

        }
    }gamePlayer_t;

    typedef struct carGraph{

        typedef enum {
            NORMAL = 0,
            RIGHT,
            LEFT
        }CarStatus;

        std::array<SDL_Rect, 10> normal_graph;
        std::array<SDL_Rect, 10> right_graph;
        std::array<SDL_Rect, 10> left_graph;
        carGraph();
        std::array<SDL_Rect, 10>& getCarGraph(CarStatus s);

    }carGraph_t;

    typedef struct gameScore{
        void ScoreInit(){

        }
    }gameScore_t;

    typedef struct gameTime{
        void TimeInit(){

        }
    }gameTime_t;

    typedef struct gameEvent{
        bool keys[3] = {false, false, false}; // 前，左，右
        int wheel = 0;

        void EventInit(){
            std::fill(keys, keys + 3, false);
            wheel = 0;
        }
    }gameEvent_t;

    typedef struct gameData{
        gamePlayer_t* player;
        gameScore_t* scorer;
        gameTime_t* timer;
        gameEvent_t* evener;

        void DataInit(){
            player = new gamePlayer_t();
            scorer = new gameScore_t();
            timer = new gameTime_t();
            evener = new gameEvent_t();

            player->PlayerInit();
            scorer->ScoreInit();
            timer->TimeInit();
            evener->EventInit();
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
