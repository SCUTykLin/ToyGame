//
// Created by YangKai.Lin on 2026/8/6.
//

#ifndef TOYPROJECTS_CONFPS_FPS_H
#define TOYPROJECTS_CONFPS_FPS_H
#include <windows.h>
#include <string>

namespace ConFPS {
    typedef struct Player {
        float fPlayerX = 8.0f;
        float fPlayerY = 8.0f;
        float fPlayerA = 0.0f;
        float fFOV = 3.14159 / 4.0;
        float fDepth = 16.0f;
    }Player_t;

    typedef struct Map {
        int nMapHeight = 16;
        int nMapWidth = 16;
        std::wstring map;
    }Map_t;

    void mainLoop();

    extern Player_t player;
    extern Map_t map;
    extern wchar_t *screen;
    extern HANDLE hConsole;
}


#endif //TOYPROJECTS_FPS_H