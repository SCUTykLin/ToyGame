//
// Created by YangKai.Lin on 2026/7/16.
//

#include "renderWindow.h"
#include "../Log.h"
#include "../../BouncyBall/bouncy_ball.h"
#include "../Entity/entity.h"

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
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }

    int RenderWindow::getRefreshRate() {
        int displayIndex = SDL_GetWindowDisplayIndex(window);
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(displayIndex, 0, &mode);
        return mode.refresh_rate;
    }

    void RenderWindow::render(Entity& p_entity) {
        SDL_Rect src;
        src.x = p_entity.getCurrentFrame().x;
        src.y = p_entity.getCurrentFrame().y;
        src.w = p_entity.getCurrentFrame().w;
        src.h = p_entity.getCurrentFrame().h;

        SDL_Rect dst;
        dst.x = p_entity.getPos().x * 4;
        dst.y = p_entity.getPos().y * 4;
        dst.w = p_entity.getCurrentFrame().w * 4;
        dst.h = p_entity.getCurrentFrame().h * 4;
        SDL_RenderCopy(renderer, p_entity.getTex(), &src, &dst);
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
