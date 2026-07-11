//
// Created by 15813 on 2026/7/12.
//

#include "renderer.h"

namespace roguelike{
    SDL_Window* Renderer::window = nullptr;
    SDL_Renderer* Renderer::renderer = nullptr;
    std::once_flag Renderer::init_flag;

    SDL_Renderer* Renderer::get_window_renderer(){
        return renderer;
    }

    SDL_Window* Renderer::get_window(){
        return window;
    }
}