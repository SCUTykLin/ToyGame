//
// Created by YangKai.Lin on 2026/7/16.
//

#ifndef TOYPROJECTS_RPG_Platformer_RENDERWINDOW_H
#define TOYPROJECTS_RPG_Platformer_RENDERWINDOW_H

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include "../Entity/entity.h"

namespace RPG_Platformer {
    class RenderWindow {
    public:
        RenderWindow(const char* p_title, int p_w, int p_h);
        void clearUp();
        void clear();
        void render(Entity& p_entity);
        void display();
        int getRefreshRate();
        SDL_Texture* loadTexture(const char* p_filePath);
        SDL_Texture* loadTexture(std::string p_filePath);
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
    };
}


#endif //TOYPROJECTS_RPG_Platformer_RENDERWINDOW_H