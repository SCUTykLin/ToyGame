//
// Created by YangKai.Lin on 2026/7/12.
//
#include "game.h"
#include "../ECS/Components.h"
#include "../App/Collision.h"

roguelike::ECS::Manager manager;
auto& player(manager.addEntity());
auto& wall(manager.addEntity());
auto& tile0(manager.addEntity());
auto& tile1(manager.addEntity());
auto& tile2(manager.addEntity());
namespace roguelike {

    Renderer Game::renderer;
    SDL_Event Game::event;
    std::vector<ECS::ColliderComponent*> Game::colliders;
    Game::Game() {

    }

    Game::~Game() {

    }

    void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen) {
        renderer.create_window(title, xpos, ypos, width, height, fullscreen, isRunning);
        player.addComponent<roguelike::ECS::TransformComponent>();
        player.addComponent<roguelike::ECS::SpriteComponent>("src/Roguelike/Assets/proj.png");
        player.addComponent<roguelike::ECS::KeyboardController>();
        player.addComponent<roguelike::ECS::ColliderComponent>("player");

        wall.addComponent<roguelike::ECS::TransformComponent>(300.0f, 300.0f, 200, 20, 1);
        wall.addComponent<roguelike::ECS::SpriteComponent>("src/Roguelike/Assets/ColTex.png");
        wall.addComponent<roguelike::ECS::ColliderComponent>("wall");
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
        renderer.render(player, wall);
    }
    void Game::clean() {
        renderer.destroy_window();
    }

}
