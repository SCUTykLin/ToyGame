//
// Created by 15813 on 2026/8/25.
//

#include "App.h"
#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#include "vec.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iterator>

#include "stb_image.h"

#define FONT_WIDTH 128
#define FONT_HEIGHT 64
#define FONT_COLS 18
#define FONT_ROWS 7
#define FONT_CHAR_WIDTH  (FONT_WIDTH  / FONT_COLS)
#define FONT_CHAR_HEIGHT (FONT_HEIGHT / FONT_ROWS)
#define FONT_SCALE 5

#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 126

namespace TextEditor {
    void scc(int code){
        if (code < 0){
            fprintf(stderr, "SDL ERROR, %s\n", SDL_GetError());
            exit(1);
        }
    }

    void* scp(void *ptr){
        if (ptr == nullptr){
            fprintf(stderr, "SDL ERROR, %s\n", SDL_GetError());
            exit(1);
        }
        return ptr;
    }

    SDL_Surface* surface_from_file(const char *file_path) {
        int width, height, n;
        unsigned char* pixels = stbi_load(file_path, &width, &height, &n, 4);
        if (pixels == nullptr) {
            fprintf(stderr, "ERROR: could not load file %s: %s\n", file_path, stbi_failure_reason());
            exit(1);
        }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        const Uint32 rmask = 0xff000000;
        const Uint32 gmask = 0x00ff0000;
        const Uint32 bmask = 0x0000ff00;
        const Uint32 amask = 0x000000ff;
#else // little endian, like x86
        const Uint32 rmask = 0x000000ff;
        const Uint32 gmask = 0x0000ff00;
        const Uint32 bmask = 0x00ff0000;
        const Uint32 amask = 0xff000000;
#endif

        const int depth = 32;
        const int pitch = 4 * width;

        return reinterpret_cast<SDL_Surface*>(scp(SDL_CreateRGBSurfaceFrom((void*)pixels, width, height, depth, pitch, rmask, gmask, bmask, amask)));

    }

    void App::run(){
        scc(SDL_Init(SDL_INIT_VIDEO));

        SDL_Window* window = reinterpret_cast<SDL_Window*>(scp(SDL_CreateWindow("Text Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE)));
        SDL_Renderer* renderer = reinterpret_cast<SDL_Renderer*>(scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)));

        bool quit = false;
        while (!quit){
            SDL_Event event = {0};
            while (SDL_PollEvent(&event)){
                switch (event.type) {
                    case SDL_QUIT:
                        quit = true;
                        break;
                }
            }
            scc(SDL_SetRenderDrawColor(renderer, 100, 0, 0, 0));
            scc(SDL_RenderClear(renderer));
            SDL_RenderPresent(renderer);
        }


        SDL_Quit();
    }
}