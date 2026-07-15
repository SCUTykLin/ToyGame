//
// Created by YangKai.Lin on 2026/7/12.
//
#include "game.h"
#include "../TextureManager/textureManager.h"
#include "../Map/map.h"
#include "../ECS/Components.h"
#include "../App/Vector2D.h"
#include "../App/Collision.h"
#include "../AssetsManager/AssetManager.h"
#include <sstream>

roguelike::ECS::Manager manager;
auto& player(manager.addEntity());
auto& wall(manager.addEntity());
auto& tile0(manager.addEntity());
auto& tile1(manager.addEntity());
auto& tile2(manager.addEntity());
namespace roguelike {

    Map* map;
    ECS::Manager manager;
    Renderer Game::renderer;
    SDL_Event Game::event;
    SDL_Rect Game::camera = { 0,0,800,640 };

    AssetManager* Game::assets = new AssetManager(&manager);

    bool Game::isRunning = false;

    auto& player(manager.addEntity());
    auto& label(manager.addEntity());

    Game::Game() {}

    Game::~Game() {}

    void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen) {
        renderer.create_window(title, xpos, ypos, width, height, fullscreen, isRunning);
        if (TTF_Init() == -1)
        {
            std::cout << "Error : SDL_TTF" << std::endl;
        }
    }
    void Game::handleEvents() {
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
        manager.refresh();
        manager.update();

        if (collision::Collision::AABB(player.getComponent<ECS::ColliderComponent>().collider,
                                       wall.getComponent<ECS::ColliderComponent>().collider)){
            player.getComponent<ECS::TransformComponent>().getVel() *= -1;
            std::cout <<"Hit Wall" << std::endl;
        }
    }

    void Game::render() {
//        renderer.render(player, wall);
    }
    void Game::clean() {
        renderer.destroy_window();
    }

}
