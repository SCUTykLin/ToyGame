//
// Created by YangKai.Lin on 2026/7/16.
//

#include "App.h"
#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "../Log.h"

#define NAME RPG_Platformer::App

void NAME::run() {
    initWindow();
}

void NAME::initWindow() {
    if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
        print("Init SDL FAILED. SDL_ERROR: ", SDL_GetError());
    }
}
