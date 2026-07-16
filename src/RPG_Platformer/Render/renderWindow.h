//
// Created by YangKai.Lin on 2026/7/16.
//

#ifndef TOYPROJECTS_RENDERWINDOW_H
#define TOYPROJECTS_RENDERWINDOW_H

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>

namespace RPG_Platformer {
    class RenderWindow {
    public:
        RenderWindow(const char* p_title, int p_w, int p_h);
        void clearUp();
        void clear();
        void render(SDL_Texture* p_tex);
        void display();
        SDL_Texture* loadTexture(const char* p_filePath);
        SDL_Texture* loadTexture(std::string p_filePath);
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
    };
}


#endif //TOYPROJECTS_RENDERWINDOW_H