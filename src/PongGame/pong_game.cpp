//
// Created by 15813 on 2026/6/30.
//

#include "pong_game.h"

namespace Pong {
    Uint32 black_color = 0x00000000;
    Uint32 white_color = 0xffffffff;
    float MOVEMENT_SPEED = 2.0f;
    const int FRAME_TARGET = 1000 / 60;
    const int LEFT_BORDER = 80;
    const int RIGHT_BORDER = 560;
    const int PL_HEIGHT = 200;
    const int TOP_INNER_BORDER = 0;
    const int BOTTOM_INNER_BORDER = 640;

}

void Pong::startPongGame(){
    Pong::main_loop();
}

void Pong::draw_rect(SDL_Surface* surface, SDL_Rect* rect, Uint32 color){
    SDL_FillRect(surface, rect, color);
}

void Pong::move_rect(SDL_Surface* surface, SDL_Rect* rect, Pong::Speed* speed){
    SDL_FillRect(surface, rect, Pong::black_color);
    rect->y += speed->y;
    rect->x += speed->x;
    SDL_FillRect(surface, rect, Pong::white_color);
}

void Pong::move_rect(SDL_Surface* surface, SDL_Rect* rect, Pong::Speed& speed){
    SDL_FillRect(surface, rect, Pong::black_color);
    rect->y += speed.y;
    rect->x += speed.x;
    SDL_FillRect(surface, rect, Pong::white_color);
}

SDL_Rect Pong::create_rect(SDL_Surface* surface, Uint32 color, int x, int y, int w, int h){
    SDL_Rect rect{x, y, w, h};
    SDL_FillRect(surface, &rect, color);
    return rect;
}

void Pong::move_player(SDL_Surface* surface, SDL_Rect* pl, int direction){
    if (direction < 0 && pl->y <= 0){
        return;
    }
    if (direction > 0 && pl->y >= (surface->h - pl->h)){
        return;
    }
    Pong::Speed speed{0, direction * Pong::MOVEMENT_SPEED};
    move_rect(surface, pl, speed);
}

void Pong::Ball::move_ball(SDL_Surface *surface, SDL_Rect* pl1, SDL_Rect* pl2) {
    if (get_x() <= Pong::LEFT_BORDER){
        if ((get_y() + get_size_y() > pl1->y) && (get_y() < pl1->y + pl1->h)){

            auto hit_fraction = ((double)(get_y() + get_size_y()) / (double)2 - (double)(pl1->y + pl1->h) / (double)2) / ((double)PL_HEIGHT / 2);
            float x_speed = -get_speed_x();
            float y_speed = (double)(hit_fraction * (double)Pong::MOVEMENT_SPEED);
            set_speed(x_speed, y_speed);
        }
    }

    if (get_x() + get_size_x() >= Pong::RIGHT_BORDER){
        if ((get_y() + get_size_y() > pl2->y) && (get_y() < pl2->y + pl2->h)){
            auto hit_fraction = ((double)(get_y() + get_size_y()) / (double)2 - (double)(pl2->y + pl2->h) / (double)2) / ((double)PL_HEIGHT/2);
            float x_speed = -get_speed_x();
            float y_speed = (double)(hit_fraction * (double)Pong::MOVEMENT_SPEED);
            set_speed(x_speed, y_speed);
        }
    }

    if (get_y() <= Pong::TOP_INNER_BORDER){
        set_speed(get_speed_x(), -get_speed_y());
    }
    if (get_y() + get_size_y() >= Pong::BOTTOM_INNER_BORDER){
        set_speed(get_speed_x(), -get_speed_y());
    }

    move_rect(surface, this->rect, this->speed);
}

int Pong::Ball::get_x() {
    return this->rect->x;
}

int Pong::Ball::get_y() {
    return this->rect->y;
}

void Pong::Ball::set_x(float x) {
    this->rect->x = x;
}

void Pong::Ball::set_y(float y) {
    this->rect->y = y;
}

int Pong::Ball::get_size_x() {
    return this->rect->w;
}

int Pong::Ball::get_size_y() {
    return this->rect->h;
}

void Pong::Ball::set_speed(float x, float y) {
    this->speed->x = x;
    this->speed->y = y;
}

float Pong::Ball::get_speed_x() {
    return this->speed->x;
}

float Pong::Ball::get_speed_y() {
    return this->speed->y;
}

SDL_Surface* Pong::create_window(std::string title, int height, int width) {
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    return surface;
}

void Pong::update_window() {
    SDL_UpdateWindowSurface(window);
}

void Pong::main_loop(){
    SDL_Surface* surface = Pong::create_window("pong game",480, 640);

    SDL_Rect pl1 = create_rect(surface, Pong::white_color, 40, 40, 40, 200);
    SDL_Rect pl2 = create_rect(surface, Pong::white_color, 560, 300, 40, 200);
    SDL_Rect border = create_rect(surface, Pong::white_color, 320, 0, 1, 480);

    // ball
    SDL_Rect ball_rect = create_rect(surface, Pong::white_color, 320, 240, 20, 20);
    Speed ball_speed{Pong::MOVEMENT_SPEED, 0};
    Ball ball;
    ball.speed = &ball_speed;
    ball.rect = &ball_rect;

    bool is_running = true;
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(nullptr);

    while(is_running){
        Uint32 frame_start = SDL_GetTicks();

        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT){
            is_running = false;
        }
        // 玩家1：W上、S下
        if (keystate[SDL_SCANCODE_W]) {
            move_player(surface, &pl1, -1);
        }
        if (keystate[SDL_SCANCODE_S]) {
            move_player(surface, &pl1, 1);
        }
        // 玩家2：上方向键、下方向键
        if (keystate[SDL_SCANCODE_UP]) {
            move_player(surface, &pl2, -1);
        }
        if (keystate[SDL_SCANCODE_DOWN]) {
            move_player(surface, &pl2, 1);
        }

        ball.move_ball(surface, &pl1, &pl2);
        draw_rect(surface, &border, Pong::white_color);
        Pong::update_window();
        // 固定帧数
        Uint32 frame_cost = SDL_GetTicks() - frame_start;
        if (FRAME_TARGET > frame_cost) {
            SDL_Delay(FRAME_TARGET - frame_cost);
        }
    }
}