//
// Created by YangKai.Lin on 2026/7/9.
//

#include "window_tool.h"

#include "../BouncyBall/bouncy_ball.h"

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
    void clear_window(SDL_Surface* surface, Uint32 color) {
        SDL_FillRect(surface, canvas, color);
    }

    void update_window() {
        SDL_UpdateWindowSurface(window);
    }

    Uint32 get_interpolated_color(Uint32 min, Uint32 max, double percentage) {
        unsigned char r1 = (min >> 16) & 0xFF;
        unsigned char r2 = (max >> 16) & 0xFF;
        unsigned char g1 = (min >> 8) & 0xFF;
        unsigned char g2 = (max >> 8) & 0xFF;
        unsigned char b1 = min & 0xFF;
        unsigned char b2 = max & 0xFF;
        double ratio = std::clamp(percentage, 0.0, 1.0);
        return (int)((r2 - r1) * ratio + r1) << 16 | (int)((g2 - g1) * ratio + g1) << 8 | (int)((b2 - b1) * ratio + b1);
    }

    void destroy_window() {
        delete canvas;
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

}
