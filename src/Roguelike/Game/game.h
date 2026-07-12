//
// Created by YangKai.Lin on 2026/7/12.
//

#ifndef TOYPROJECTS_ROGUELIKE_GAME_H
#define TOYPROJECTS_ROGUELIKE_GAME_H

#include "SDL2/SDL.h"
#include "../Renderer/renderer.h"
#include "SDL_image.h"

namespace roguelike {
    class Game {
    public:
        Game();
        ~Game();

        void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
        void handleEvents();
        void update();
        void render();
        void clean();

        bool running() const {
            return isRunning;
        }
    private:
        bool isRunning;
        SDL_Texture* playerTex;
        Renderer renderer;
    };
}


#endif //TOYPROJECTS_ROGUELIKE_GAME_H