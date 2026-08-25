//
// Created by 15813 on 2026/8/25.
//

#include "App.h"
#include <SDL.h>
#include <cstdio>
#include <cstdlib>

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