//
// Created by 15813 on 2026/8/6.
//

#ifndef TOYPROJECTS_CONFPS_SDL_FPS_H
#define TOYPROJECTS_CONFPS_SDL_FPS_H

#include <SDL2/SDL.h>

namespace ConFPS {
    class FPS_Window {
    public:
        typedef enum {
            CEILING,
            WALL,
            FLOOR
        }category_t;
        void init_window(int width, int height);
        void SetDistance(int x, int y, float value);
        void SetCategory(int x, int y, category_t cate);
    private:
        static SDL_Window *window;
        static SDL_Surface *surface;
        static SDL_Renderer *renderer;
        static float* distance;
        static category_t* category;
        static int nScreenWidth;
        static int nScreenHeight;
    };
}

#endif //TOYPROJECTS_CONFPS_SDL_FPS_H
