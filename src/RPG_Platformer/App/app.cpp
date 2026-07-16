//
// Created by YangKai.Lin on 2026/7/16.
//

#include "app.h"
#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "../Render/renderWindow.h"
#include "../Log.h"

#define NAME RPG_Platformer::App

void NAME::run() {
    initWindow();
    RenderWindow window("RPG Platformer", 1280, 720);

    bool gameRunning = true;

    SDL_Texture* grassTexture = window.loadTexture(ASSET_DIR + "ground_grass_1.png");

    SDL_Event event;
    while (gameRunning) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
            }
        }
        window.clear();
        window.render(grassTexture);
        window.display();
    }

    window.clearUp();

}

void NAME::initWindow() {
    if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
        print("Init SDL FAILED. SDL_ERROR: ", SDL_GetError());
    }

    if (!IMG_Init(IMG_INIT_PNG)) {
        print("Init SDL_image FAILED. SDL_ERROR: ", SDL_GetError());
    }
}
