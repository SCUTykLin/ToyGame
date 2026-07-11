//
// Created by 15813 on 2026/7/12.
//

#include "app.h"
namespace roguelike {
    void App::start() {
        renderer.create_window<W, H>("Roguelike RPG");
        auto sdl_renderer = renderer.get_window_renderer();
        SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
        SDL_RenderPresent(sdl_renderer);
        SDL_Delay(3000);
    }
}