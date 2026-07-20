//
// Created by 15813 on 2026/7/17.
//

#include "Engine.h"
#include "../Graphics/TextureManager.h"
#include <iostream>
#include "../Characters/Warrior.h"
#include "../Inputs/Inputs.h"
#include "../Map/MapParser.h"
#include "../Time/Time.h"
#include "../Camera/Camera.h"

namespace Madsycode::Engine{
    Engine* Engine::s_Instance = nullptr;
    Character::Warrior* player = nullptr;

    bool Engine::Init() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0 && IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != 0) {
            SDL_Log("Failed to initialize SDL_%s", IMG_GetError());
            return false;
        }
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        m_Window = SDL_CreateWindow("Soft Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
        if (m_Window == nullptr) {
            SDL_Log("Failed to create window: %s", SDL_GetError());
            return false;
        }

        m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (m_Renderer == nullptr) {
            SDL_Log("Failed to create renderer: %s", SDL_GetError());
            return false;
        }
        SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
        if (Map::MapParser::GetInstance()->Load()) {
            std::cout << "load map ..." << std::endl;
        }

        m_LevelMap = Map::MapParser::GetInstance()->GetMaps("MAP");

        Texture::TextureManager::GetInstance()->Load("player", MADSYCODE_RESOURCE"Assets/character/anim/idle/spritesheet.png");
        Texture::TextureManager::GetInstance()->Load("player_run", MADSYCODE_RESOURCE"Assets/character/anim/run/spritesheet.png");
        Texture::TextureManager::GetInstance()->Load("player_jump", MADSYCODE_RESOURCE"Assets/character/anim/jump/spritesheet.png");
        Texture::TextureManager::GetInstance()->Load("player_fall", MADSYCODE_RESOURCE"Assets/character/anim/fall/spritesheet.png");
        Texture::TextureManager::GetInstance()->Load("bg", MADSYCODE_RESOURCE"Assets/Background/Clouds.png");

        player = new Character::Warrior(new Object::Properties("player", 100, 200, 131, 142));
        Camera::Camera::GetInstance()->SetTarget(player->GetOrigin());
        return m_IsRunning = true;
    }

    void Engine::Clean() {
        Texture::TextureManager::GetInstance()->Clean();
        SDL_DestroyRenderer(m_Renderer);
        SDL_DestroyWindow(m_Window);
        IMG_Quit();
        SDL_Quit();
    }

    void Engine::Quit() {
        m_IsRunning = false;
        SDL_DestroyWindow(m_Window);
        SDL_DestroyRenderer(m_Renderer);
        SDL_Quit();
    }

    void Engine::Update() {
        auto dt = Time::Timer::GetInstance()->GetDeltaTime();
        m_LevelMap->Update();
        player->Update(dt);
        Camera::Camera::GetInstance()->Update(dt);
    }

    void Engine::Render() {
        SDL_SetRenderDrawColor(m_Renderer, 124, 218, 254, 255);
        SDL_RenderClear(m_Renderer);

        Texture::TextureManager::GetInstance()->Draw("bg", 0, 0, 1920, 1080);

        m_LevelMap->Render();

        player->Draw();
        SDL_RenderPresent(m_Renderer);
    }

    void Engine::Events() {
        Event::Input::GetInstance()->Listen();
    }
}
