//
// Created by YangKai.Lin on 2026/7/16.
//

#include "renderWindow.h"
#include "../Log.h"

namespace RPG_Platformer {
    RenderWindow::RenderWindow(const char *p_title, int p_w, int p_h) : window(nullptr), renderer(nullptr){
        window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            print("Window failed to init. Error. ", SDL_GetError());
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // SDL_RENDERER_ACCELERATED: use gpu for rendering

    }

    void RenderWindow::clearUp() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
    }

    void RenderWindow::clear() {
        SDL_RenderClear(renderer);
    }

    void RenderWindow::render(SDL_Texture *p_tex) {
        SDL_RenderCopy(renderer, p_tex, NULL, NULL);
    }

    void RenderWindow::display() {
        SDL_RenderPresent(renderer);
    }

    SDL_Texture * RenderWindow::loadTexture(const char *p_filePath) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, p_filePath);
        if (texture == nullptr) {
            print("texture failed to load from: ", p_filePath, "Error: ", SDL_GetError());
        }
        return texture;
    }

    SDL_Texture * RenderWindow::loadTexture(std::string p_filePath) {
        return loadTexture(p_filePath.c_str());
    }
}
