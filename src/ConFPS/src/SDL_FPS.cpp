#include "SDL_FPS.h"
#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

namespace ConFPS {

    SDL_Window* FPS_Window::window = nullptr;
    SDL_Surface* FPS_Window::surface = nullptr;
    SDL_Renderer* FPS_Window::renderer = nullptr;
    SDL_Texture* FPS_Window::framebufferTexture = nullptr;

    int FPS_Window::nScreenWidth = 0;
    int FPS_Window::nScreenHeight = 0;

    bool* FPS_Window::boundary = nullptr;
    float* FPS_Window::distance = nullptr;
    FPS_Window::category_t* FPS_Window::category = nullptr;

    bool FPS_Window::sdl_running = false;

    constexpr int WINDOW_WIDTH  = 1280;
    constexpr int WINDOW_HEIGHT = 720;

    std::vector<uint32_t> FPS_Window::pixels;

    void FPS_Window::ResetDistance(){
        std::fill(distance, distance + nScreenWidth * nScreenHeight, std::numeric_limits<float>::infinity());
    }

    void FPS_Window::ResetCategory(){
        std::fill(category, category + nScreenWidth * nScreenHeight, CEILING);
    }

    void FPS_Window::ResetBoundary(){
        std::fill(boundary, boundary + nScreenWidth * nScreenHeight, false);
    }

    void FPS_Window::SetDistance(int x, int y, float value){
        distance[y * nScreenWidth + x] = value;
    }

    void FPS_Window::SetCategory(int x, int y, category_t value){
        category[y * nScreenWidth + x] = value;
    }

    void FPS_Window::SetBoundary(int x, int y, bool value){
        boundary[y * nScreenWidth + x] = value;
    }

    void FPS_Window::init_window(int width, int height){
        nScreenWidth = width;
        nScreenHeight = height;

        distance = new float[nScreenWidth * nScreenHeight];
        category = new category_t[nScreenWidth * nScreenHeight];
        boundary = new bool[nScreenWidth * nScreenHeight];

        std::fill(distance, distance + nScreenWidth * nScreenHeight, std::numeric_limits<float>::infinity());
        std::fill(category, category + nScreenWidth * nScreenHeight, CEILING);
        std::fill(boundary, boundary + nScreenWidth * nScreenHeight, false);
        pixels.resize(nScreenWidth * nScreenHeight);

        if (SDL_Init(SDL_INIT_VIDEO) < 0){
            std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
            return;
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        window = SDL_CreateWindow("FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window){
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
            return;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer){
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
            return;
        }
        framebufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, nScreenWidth, nScreenHeight);
        if (!framebufferTexture){
            std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << '\n';
            return;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        sdl_running = true;
    }

    void FPS_Window::control(bool& is_running, float& scale){
        SDL_Event event;
        while (SDL_PollEvent(&event) && sdl_running){
            if (event.type == SDL_QUIT){
                stop();
                is_running = false;
                return;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
                stop();
                is_running = false;
                return;
            }

            if (event.type == SDL_MOUSEWHEEL)
            {
                if (event.wheel.y > 0)
                {
                    scale += 1;
                }
                else if (event.wheel.y < 0)
                {
                    scale -= 1;
                }

                scale = std::clamp(scale, 5.0f, 30.0f);
            }

        }
    }

    inline uint32_t RGB(uint8_t r, uint8_t g, uint8_t b){
        return 0xFF000000u | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
    }

    inline uint8_t GetR(uint32_t color){
        return (color >> 16) & 0xFF;
    }

    inline uint8_t GetG(uint32_t color){
        return (color >> 8) & 0xFF;
    }

    inline uint8_t GetB(uint32_t color){
        return color & 0xFF;
    }

    void FPS_Window::render(Player_t *player, Map_t *map, float scale){
        if (!sdl_running) {
            return;
        }

        if (!renderer || !framebufferTexture)
            return;

        for (int y = 0; y < nScreenHeight; ++y){
            for (int x = 0; x < nScreenWidth; ++x){
                const int index = y * nScreenWidth + x;

                category_t cate = category[index];
                float d = distance[index];

                uint32_t color = RGB(0, 0, 0);
                float brightness = 1.0f / (1.0f + d * 0.15f);
                brightness = std::clamp(brightness, 0.1f, 1.0f);
                switch (cate){
                    case CEILING:{
                        color = RGB(110* brightness, 165* brightness, 200 * brightness);
                        break;
                    }
                    case WALL:{
                        uint8_t shade = static_cast<uint8_t>(220.0f * brightness);
                        color = RGB(shade, shade, shade);
                        break;
                    }
                    case FLOOR:{
                        color = RGB(125 * brightness, 85 * brightness, 50 * brightness);
                        break;
                    }
                    default:{
                        color = RGB(255 * brightness, 0 * brightness, 255 * brightness);
                        break;
                    }
                }
                pixels[index] = color;
            }
        }

        average_pixels();

        if (SDL_UpdateTexture(framebufferTexture, nullptr, pixels.data(),nScreenWidth *sizeof(uint32_t)) != 0){
            std::cerr << "SDL_UpdateTexture failed: " << SDL_GetError() << '\n';
            return;
        }

        SDL_SetRenderDrawColor(renderer,0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, framebufferTexture, nullptr, nullptr);

        render_map(player, map, scale);

        SDL_RenderPresent(renderer);
    }

    void FPS_Window::average_pixels() {
        for (int y = 0; y < nScreenHeight; ++y) {
            int ceiling_count = 0, floor_count = 0;
            uint32_t   ceiling_color_r = 0, floor_color_r = 0;
            uint32_t   ceiling_color_g = 0, floor_color_g = 0;
            uint32_t   ceiling_color_b = 0, floor_color_b = 0;
            for (int x = 0; x < nScreenWidth; ++x) {
                if (category[y * nScreenWidth + x] == CEILING) {
                    ++ceiling_count;
                    ceiling_color_r += GetR(pixels[y * nScreenWidth + x]);
                    ceiling_color_g += GetG(pixels[y * nScreenWidth + x]);
                    ceiling_color_b += GetB(pixels[y * nScreenWidth + x]);
                }else if (category[y * nScreenWidth + x] == FLOOR) {
                    ++floor_count;
                    floor_color_r += GetR(pixels[y * nScreenWidth + x]);
                    floor_color_g += GetG(pixels[y * nScreenWidth + x]);
                    floor_color_b += GetB(pixels[y * nScreenWidth + x]);
                }
            }
            if (ceiling_count) {
                ceiling_color_r /= ceiling_count;
                ceiling_color_g /= ceiling_count;
                ceiling_color_b /= ceiling_count;
            }

            if (floor_count) {
                floor_color_r /= floor_count;
                floor_color_g /= floor_count;
                floor_color_b /= floor_count;
            }

            auto ceiling_color = RGB(ceiling_color_r, ceiling_color_g, ceiling_color_b);
            auto floor_color = RGB(floor_color_r, floor_color_g, floor_color_b);

            for (int x = 0; x < nScreenWidth; ++x) {
                if (category[y * nScreenWidth + x] == CEILING) {
                    pixels[y * nScreenWidth + x] = ceiling_color;
                }else if (category[y * nScreenWidth + x] == FLOOR) {
                    pixels[y * nScreenWidth + x] = floor_color;
                }else if (category[y * nScreenWidth + x] == WALL && boundary[y * nScreenWidth + x]) {
                    pixels[y * nScreenWidth + x] = RGB(70, 70, 75);
                }
            }
        }
    }

    inline bool is_wall(int x, int y, const Map_t* map){
        if (x < 0 || x >= map->nMapWidth || y < 0 || y >= map->nMapHeight){
            return false;
        }
        return map->map[y * map->nMapWidth + x] == L'#';
    }

    inline bool is_vertical(int x, int y, const Map_t* map){
        return is_wall(x, y - 1, map) || is_wall(x, y + 1, map);
    }

    inline bool is_horizontal(int x, int y, const Map_t* map){
        return is_wall(x - 1, y, map) || is_wall(x + 1, y, map);
    }

    void FPS_Window::render_map(Player_t *player, Map_t *map, float scale) {
        SDL_SetRenderDrawColor(renderer, 180, 70, 65, 255);
        const int cell = static_cast<int>(scale);
        const int thickness = static_cast<int>(scale * 0.5f);
        for (int x = 0; x < map->nMapWidth; ++x ) {
            for (int y = 0; y < map->nMapHeight; ++y) {
                if (!is_wall(x, y, map)) continue;
                bool horizontal = is_horizontal(x, y, map);
                bool vertical = is_vertical(x, y, map);
                int baseX = static_cast<int>(x * scale);
                int baseY = static_cast<int>(y * scale);

                if (horizontal){
                    SDL_Rect rect{baseX, baseY, cell, thickness};
                    SDL_RenderFillRect(renderer, &rect);
                }

                if (vertical){
                    SDL_Rect rect{baseX, baseY, thickness, cell};
                    SDL_RenderFillRect(renderer,&rect);
                }
            }
        }
        render_triangle(player->fPlayerX * scale - thickness / 2, player->fPlayerY * scale - thickness / 2, thickness, -player->fPlayerA);
    }

    void FPS_Window::render_triangle(float x, float y, float scale, float angle) {
        SDL_SetRenderDrawColor(renderer, 40, 150, 18, 255);
        float c = cosf(angle);
        float s = sinf(angle);
        SDL_Vertex vertices[4] = {
            {
                {0.0f, 0.0f},
                {40, 150, 18, 255},
                {0.0f, 0.0f}
            },
            {
                {0.0f, scale},
                {40, 150, 18, 255},
                {0.0f, 0.0f}
            },
            {
                {-scale, -scale},
                {40, 150, 18, 255},
                {0.0f, 0.0f}
            },
            {
                {scale, -scale},
                {40, 150, 18, 255},
                {0.0f, 0.0f}
            }
        };

        for (int i = 0; i < 4; ++i) {
            float px = vertices[i].position.x;
            float py = vertices[i].position.y;

            float rx = px * c - py * s;
            float ry = px * s + py * c;

            vertices[i].position.x = rx + x;
            vertices[i].position.y = ry + y;
        }

        int indices[6] = {1, 2, 0, 1, 3, 0};
        SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
    }

    void FPS_Window::destory() {
        if (framebufferTexture){
            SDL_DestroyTexture(framebufferTexture);
            framebufferTexture = nullptr;
        }

        if (renderer){
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window){
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        delete[] distance;
        distance = nullptr;

        delete[] category;
        category = nullptr;
        pixels.clear();
        SDL_Quit();
        sdl_running = false;
    }
}
