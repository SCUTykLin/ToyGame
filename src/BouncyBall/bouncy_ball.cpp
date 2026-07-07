//
// Created by YangKai.Lin on 2026/7/7.
//

#include "bouncy_ball.h"

namespace bouncy {
    constexpr Uint32 black_color = 0x00000000;
    constexpr Uint32 white_color = 0xffffffff;

    void start_bouncy(){
        main_loop();
        window_tool::delete_assets();
    }

    void Physic_Circle::step() {
        this->x += this->v_x;
        this->y += this->v_y;
        this->v_x += GRAVITY;
    }

    Physic_Circle::Physic_Circle(int x, int y, int radius, int v_x, int v_y) : x(x), y(y), radius(radius), v_x(v_x), v_y(v_y) {
        sdl_circle = new window_tool::SDL_Circle(x, y, radius);
    }

    Physic_Circle::~Physic_Circle() {

    }

    void main_loop() {
        SDL_Surface* surface = window_tool::create_window("BouncyBall", WIDTH, HEIGHT);
        Physic_Circle* p_circle = new Physic_Circle(200, 200, 10, 0, 0);
        window_tool::draw_circle(surface, p_circle, white_color);
        window_tool::SDL_Circle* circle = new window_tool::SDL_Circle(200, 200, 10);
        auto re = window_tool::create_rect(10, 10, 10, 10);
        window_tool::update_window();
        std::cout << "开始删除" << std::endl;
        window_tool::delete_assets(circle);
        std::cout << "删除结束" << std::endl;
        // SDL_Event event;
        // bool simulation_running = true;
        // while(simulation_running) {
        //     while (SDL_PollEvent(&event)) {
        //         if (event.type == SDL_QUIT) {
        //             simulation_running = false;
        //         }
        //     }
        //
        // }
    }


}