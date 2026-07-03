//
// Created by 15813 on 2026/6/30.
//

#include "pong_game.h"

void Pong::startPongGame(){
    Pong::main_loop();
}

void move_rect(SDL_Surface* surface, SDL_Rect* rect, int down){
    SDL_FillRect(surface, rect, Pong::black_color);
    rect->y += 10;
    SDL_FillRect(surface, rect, Pong::white_color);
}

void create_rect(SDL_Surface* surface, SDL_Rect* rect, Uint32 color){

}

void Pong::main_loop(){
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("pong game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect rect{40, 40, 40, 200};
    SDL_FillRect(surface, &rect, Pong::white_color);

    bool is_running = true;
    SDL_Event event;
    while(is_running){
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT){
            is_running = false;
        }
        if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN){
            move_rect(surface, &rect, Pong::MOVEMENT_SPEED);
        }
        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_UP){
            std::cout << "key up" << std::endl;
        }
        SDL_UpdateWindowSurface(window);

    }
}