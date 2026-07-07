//
// Created by 15813 on 2026/6/30.
//

#ifndef TOYPROJECTS_PONG_GAME_H
#define TOYPROJECTS_PONG_GAME_H

#include <iostream>
#include <SDL.h>
#include <SDL_main.h>

namespace Pong {
    extern Uint32 black_color;
    extern Uint32 white_color;
    extern float MOVEMENT_SPEED;
    extern const int FRAME_TARGET;
    extern const int LEFT_BORDER;
    extern const int RIGHT_BORDER;
    extern const int PL_HEIGHT;
    extern const int TOP_INNER_BORDER;
    extern const int BOTTOM_INNER_BORDER;

    static SDL_Window* window = nullptr;

    typedef struct Speed{
        float x;
        float y;
    }Speed;

    typedef struct Ball{
        Speed* speed;
        SDL_Rect* rect;

        int get_x();
        int get_y();
        void set_x(float x);
        void set_y(float y);
        int get_size_x();
        int get_size_y();
        void set_speed(float x, float y);
        float get_speed_x();
        float get_speed_y();
        void move_ball(SDL_Surface *surface, SDL_Rect* pl1, SDL_Rect* pl2);
    }Ball;

    void startPongGame();

    void main_loop();

    void draw_rect(SDL_Surface* surface, SDL_Rect* rect, Uint32 color);

    void move_rect(SDL_Surface* surface, SDL_Rect* rect, Pong::Speed& speed);

    SDL_Rect create_rect(SDL_Surface* surface, Uint32 color, int x, int y, int w, int h);

    void move_player(SDL_Surface* surface, SDL_Rect* pl, int direction);

    void move_rect(SDL_Surface* surface, SDL_Rect* rect, Pong::Speed* speed);

    SDL_Surface* create_window(std::string title, int height, int width);

    void update_window();
}
#endif //TOYPROJECTS_PONG_GAME_H
