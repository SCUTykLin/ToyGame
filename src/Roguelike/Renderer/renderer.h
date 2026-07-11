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
        template<std::size_t W, std::size_t H>
        void create_window(std::string&& title){
            std::call_once(init_flag, [](std::string& title)->void{
                window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
                renderer = SDL_CreateRenderer(window, -1, 0);
            }, title);
        }

        SDL_Renderer* get_window_renderer();

        SDL_Window* get_window();

    private:
        static SDL_Window *window;
        static SDL_Renderer *renderer;
        static std::once_flag init_flag;
    };

}

#endif //TOYPROJECTS_ROGUELIKE_RENDERER_H
