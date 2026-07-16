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

        assets->AddTexture("terrain", "src/Roguelike/Assets/terrain_ss.png");
        assets->AddTexture("player", "src/Roguelike/Assets/player_anims.png");
        assets->AddTexture("projectile", "src/Roguelike/Assets/proj.png");

        assets->AddFont("arial", "src/Roguelike/Assets/arial.ttf", 16);

        map = new Map("terrain", 3, 32);
        map->LoadMap("src/Roguelike/Assets/map.map", 25, 20);

        player.addComponent<ECS::TransformComponent>(800.0f, 640.0f, 32, 32, 4);
        player.addComponent<ECS::SpriteComponent>("player", true);
        player.addComponent<ECS::KeyboardController>();
        player.addComponent<ECS::ColliderComponent>("player");
        player.addGroup(groupPlayers);

        SDL_Color white = { 255, 255, 255, 255 };
        label.addComponent<UILabel>(10, 10, "Test String", "arial", white);
        assets->CreateProjectile(calculate::Vector2D(600, 600), calculate::Vector2D(2,0),200, 2, "projectile");
        assets->CreateProjectile(calculate::Vector2D(600, 620), calculate::Vector2D(2, 0), 200, 2, "projectile");
        assets->CreateProjectile(calculate::Vector2D(400, 600), calculate::Vector2D(2, 1), 200, 2, "projectile");
        assets->CreateProjectile(calculate::Vector2D(600, 600), calculate::Vector2D(2, -1), 200, 2, "projectile");
    }


    auto& tiles(manager.getGroup(Game::groupMap));
    auto& players(manager.getGroup(Game::groupPlayers));
    auto& colliders(manager.getGroup(Game::groupColliders));
    auto& projectiles(manager.getGroup(Game::groupProjectiles));

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
        SDL_Rect playerCol = player.getComponent<ECS::ColliderComponent>().collider;
        calculate::Vector2D playerPos = player.getComponent<ECS::TransformComponent>().getPos();

        std::stringstream ss;
        ss << "Player position: " << playerPos;
        label.getComponent<UILabel>().SetLabelText(ss.str(), "arial");

        manager.refresh();
        manager.update();

        for (auto& c : colliders)
        {
            SDL_Rect cCol = c->getComponent<ECS::ColliderComponent>().collider;
            if (collision::Collision::AABB(cCol, playerCol))
            {
                player.getComponent<ECS::TransformComponent>().getPos() = playerPos;
            }
        }

        for (auto& p : projectiles)
        {
            if (collision::Collision::AABB(player.getComponent<ECS::ColliderComponent>().collider, p->getComponent<ECS::ColliderComponent>().collider))
            {
                std::cout << "Hit player!" << std::endl;
                p->destroy();
            }
        }

        camera.x = static_cast<int>(player.getComponent<ECS::TransformComponent>().getPos().x - 400);
        camera.y = static_cast<int>(player.getComponent<ECS::TransformComponent>().getPos().y - 320);

        if (camera.x < 0)
            camera.x = 0;
        if (camera.y < 0)
            camera.y = 0;
        if (camera.x > camera.w)
            camera.x = camera.w;
        if (camera.y > camera.h)
            camera.y = camera.h;
    }

    void Game::render() {
        renderer.render(tiles, colliders, players,projectiles, label);
    }
    void Game::clean() {
        renderer.destroy_window();
    }

}
