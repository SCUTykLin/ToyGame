//
// Created by YangKai.Lin on 2026/7/16.
//

#include "app.h"
#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "../Render/renderWindow.h"
#include "../Log.h"
#include "../Entity/entity.h"
#include "../Utils/utils.h"

#define WIDTH 1280
#define HEIGHT 720
#define SCALE 4
#define NAME RPG_Platformer::App

void NAME::run() {
    initWindow();
    RenderWindow window("RPG Platformer", WIDTH, HEIGHT);
    int windowRateRefreshRate = window.getRefreshRate();
    print("windowRateRefreshRate = ", windowRateRefreshRate);

    bool gameRunning = true;

    SDL_Texture* grassTexture = window.loadTexture(ASSET_DIR + "ground_grass_1.png");
    SDL_Texture* player = window.loadTexture(ASSET_DIR + "hulking_knight.png");

    std::vector<Entity> entitiees;
    int y = HEIGHT / SCALE - 32;
    for (int i = 0; i < WIDTH / (SCALE * 32); ++i)
        entitiees.emplace_back(Vector2f(i * 32, y), grassTexture);

    entitiees.emplace_back(Vector2f(100, y-32), player);

    const float timeStep = 0.01f;
    float accumulator = 0.0f;
    float currentTime = Utils::hireTimeInSeconds();

    SDL_Event event;
    while (gameRunning) {
        int startTicks = SDL_GetTicks();
        float newTime = Utils::hireTimeInSeconds();
        float frameTime = newTime - currentTime;

        currentTime = newTime;
        accumulator += frameTime;

        while (accumulator >= timeStep) {
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    gameRunning = false;
                }
            }
            accumulator -= timeStep;
        }

        const float alpha = accumulator / timeStep;

        window.clear();
        entitiees.back().update();
        for (Entity& e : entitiees)
        {
            window.render(e);
        }
        window.display();
        int frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < 1000 / windowRateRefreshRate) {
            SDL_Delay(1000 / windowRateRefreshRate - frameTicks);
        }
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
