//
// Created by 15813 on 2026/7/12.
//

#include "renderer.h"

namespace roguelike{
    SDL_Window* Renderer::window = nullptr;
    SDL_Renderer* Renderer::renderer = nullptr;
    std::once_flag Renderer::init_flag;

    void Renderer::create_window(const char* title, int xpos, int ypos, int width, int height, bool fullscreen, bool& isRunning){

        int flags = 0;
        if (fullscreen) {
            flags = SDL_WINDOW_FULLSCREEN;
        }

        std::call_once(init_flag, [&]()->void{
            if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
                std::cout << "Subsystems Initialised! ..." << std::endl;
                window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
                if (window) {
                    std::cout << "Window created!" << std::endl;
                }
                renderer = SDL_CreateRenderer(window, -1, 0);
                if (renderer) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    std::cout << "Renderer created!" << std::endl;
                }
                isRunning = true;
            }
            else
                isRunning = false;
        });
    }

    void Renderer::destroy_window() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        std::cout << "Game Destroyed!" << std::endl;
    }

    SDL_Renderer* Renderer::get_window_renderer(){
        return renderer;
    }

    SDL_Window* Renderer::get_window(){
        return window;
    }
}