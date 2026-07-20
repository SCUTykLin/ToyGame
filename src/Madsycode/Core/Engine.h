//
// Created by 15813 on 2026/7/17.
//

#ifndef TOYPROJECTS_Madsycode_ENGINE_H
#define TOYPROJECTS_Madsycode_ENGINE_H

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include "../Map/GameMap.h"
#include "../Assets/Animation_Config.h"

#define MADSYCODE_RESOURCE "src/Madsycode/"

namespace Madsycode::Engine {
    class Engine {
    public:
        static Engine* GetInstance(){
            return s_Instance = (s_Instance != nullptr) ? s_Instance : new Engine();
        }
        bool Init();
        void Clean();
        void Quit();
        void Update();
        void Render();
        void Events();

        inline bool isRunning(){return m_IsRunning;}
        inline SDL_Renderer* GetRenderer(){return m_Renderer;}
        inline SDL_Window* GetWindow(){return m_Window;}
        inline Map::GameMap* GetMap() {return m_LevelMap;}
    private:
        Engine(){};
        bool m_IsRunning;

        Map::GameMap* m_LevelMap;
        SDL_Window* m_Window;
        SDL_Renderer* m_Renderer;
        static Engine* s_Instance;
    };
}


#endif //TOYPROJECTS_Madsycode_ENGINE_H
