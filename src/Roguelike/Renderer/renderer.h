//
// Created by 15813 on 2026/7/12.
//

#ifndef TOYPROJECTS_ROGUELIKE_RENDERER_H
#define TOYPROJECTS_ROGUELIKE_RENDERER_H

#include "SDL2/SDL.h"
#include <iostream>
#include <mutex>

namespace roguelike {

    class Renderer {
    public:
        void create_window(const char* title, int xpos, int ypos, int width, int height, bool fullscreen, bool& isRunning);

        SDL_Renderer* get_window_renderer();

        SDL_Window* get_window();

        template <typename... GameObjects>
        void render(GameObjects&&... gameObjects) {
            SDL_RenderClear(renderer);


            SDL_RenderPresent(renderer);
        }

        void destroy_window();

    private:
        static SDL_Window *window;
        static SDL_Renderer *renderer;
        static std::once_flag init_flag;
    };

}

#endif //TOYPROJECTS_ROGUELIKE_RENDERER_H
