//
// Created by YangKai.Lin on 2026/7/12.
//

#include "game.h"

#include "../TextureManager/textureManager.h"

namespace roguelike {
    Game::Game() {

    }

    Game::~Game() {

    }

    void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen) {
        renderer.create_window(title, xpos, ypos, width, height, fullscreen, isRunning);
        playerTex = TextureManager::LoadTexture("src/Roguelike/Assets/proj.png", renderer.get_window_renderer());
    }
    void Game::handleEvents() {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            default:
                break;
        }
    }


    void Game::update() {
    }

    void Game::render() {
        renderer.render(playerTex);
    }
    void Game::clean() {
        renderer.destroy_window();
    }

}
