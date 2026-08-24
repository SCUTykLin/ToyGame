//
// Created by YangKai.Lin on 2026/8/6.
//

#ifndef TOYPROJECTS_CONFPS_FPS_H
#define TOYPROJECTS_CONFPS_FPS_H
#include <windows.h>
#include "common.h"

namespace ConFPS {
    void mainLoop();
    extern Player_t player;
    extern Map_t map;
    extern wchar_t *screen;
    extern HANDLE hConsole;
}


#endif //TOYPROJECTS_FPS_H