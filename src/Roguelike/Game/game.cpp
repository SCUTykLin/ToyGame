//
// Created by YangKai.Lin on 2026/7/12.
//

#include "game.h"

#include "../TextureManager/textureManager.h"
#include "../GameObject/gameObject.h"

roguelike::GameObject* player = nullptr;

namespace roguelike {

    Renderer Game::renderer;

    Game::Game() {

    }

    Game::~Game() {

    }

    void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen) {
        renderer.create_window(title, xpos, ypos, width, height, fullscreen, isRunning);
        player = new roguelike::GameObject("src/Roguelike/Assets/proj.png", 0, 0);
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
        player->Update();
    }

    void Game::render() {
        renderer.render(player);
    }
    void Game::clean() {
        renderer.destroy_window();
    }

}
