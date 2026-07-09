//
// Created by YangKai.Lin on 2026/7/9.
//

#include "window_tool.h"
namespace fluid::window_tool {
    SDL_Window* window = nullptr;
    SDL_Rect* canvas = nullptr;
    SDL_Surface* create_window(const std::string& title, int width, int height) {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow(title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, 0);
        SDL_Surface* surface = SDL_GetWindowSurface(window);

        canvas = new SDL_Rect(0, 0, surface->w, surface->h);
        return surface;
    }

    // 刷新canvas，覆盖前一帧
    void clear_window(SDL_Surface* surface, Uint32 color=0x00000000) {
        SDL_FillRect(surface, canvas, color);
    }

    void update_window() {
        SDL_UpdateWindowSurface(window);
    }
}