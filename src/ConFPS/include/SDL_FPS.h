//
// Created by 15813 on 2026/8/6.
//

#ifndef TOYPROJECTS_CONFPS_SDL_FPS_H
#define TOYPROJECTS_CONFPS_SDL_FPS_H

#include <SDL2/SDL.h>
#include <vector>
#include "common.h"

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
        void SetBoundary(int x, int y, bool value);
        void ResetDistance();
        void ResetBoundary();
        void ResetCategory();
        void render(Player_t *player, Map_t *map, float scale);
        void control(bool& is_running, float& scale);
        void destory();
        void average_pixels();
        void render_map(Player_t* player, Map_t* map, float scale);
        void render_triangle(float x, float y, float scale, float angle);

        inline bool run() {return sdl_running;}
        inline void stop() {sdl_running = false; destory();}
    private:
        static SDL_Window *window;
        static SDL_Surface *surface;
        static SDL_Renderer *renderer;
        static SDL_Texture* framebufferTexture;
        static float* distance;
        static category_t* category;
        static bool* boundary;
        static int nScreenWidth;
        static int nScreenHeight;
        static std::vector<uint32_t> pixels;

        static bool sdl_running;
    };
}

#endif //TOYPROJECTS_CONFPS_SDL_FPS_H
