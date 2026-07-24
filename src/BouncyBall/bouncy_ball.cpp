//
// Created by YangKai.Lin on 2026/7/7.
//

#include "bouncy_ball.h"

namespace bouncy {
    constexpr Uint32 black_color = 0x00000000;
    constexpr Uint32 white_color = 0xffffffff;
    constexpr Uint32 color_background = 0x0f0f0f0f;
    constexpr int FRAME_TARGET = 1000 / 60;
    SDL_Surface* surface = nullptr;

    void start_bouncy(){
        main_loop();
        window_tool::delete_assets();
        window_tool::finish_window(surface);
    }

    void Physic_Circle::update_velocity() {
        this->v_x += GRAVITY;
    }

    void Physic_Circle::delete_trajectory() {
        trajectory_length--;
    }

    void Physic_Circle::add_trajectory(int x, int y, int size) {
        if (trajectory_length >= TRAJECTORY_SIZE) {
            for (int idx = (current_index - trajectory_length + 1 + TRAJECTORY_SIZE) % TRAJECTORY_SIZE, size_ = 0;
                size_ < trajectory_length - 2;
                ++size_, idx = (idx + 1) % TRAJECTORY_SIZE) {
                trajectory[idx]->resetTrajectory(
                    trajectory[(idx + 1) % TRAJECTORY_SIZE]->x,
                    trajectory[(idx + 1) % TRAJECTORY_SIZE]->y,
                    trajectory[(idx + 1) % TRAJECTORY_SIZE]->size,
                    trajectory[idx]->alpha); // alpha不需要改变
            }
            trajectory[current_index]->resetTrajectory(x, y, size, TRAJECTORY_SIZE);
        }else {
            trajectory[current_index] = new Trajectory(x, y, size);
            trajectory_length++;
        }
        current_index = (current_index + 1) % TRAJECTORY_SIZE;
    }

    void Physic_Circle::update_trajectory() {
        int idx = 0, _ = 0;
        for (idx = (current_index - 1 + TRAJECTORY_SIZE) % TRAJECTORY_SIZE, _ = 0; _ < trajectory_length; ++_, idx = (idx - 1 + TRAJECTORY_SIZE) % TRAJECTORY_SIZE) {
            trajectory[idx]->alpha --;
            if (!trajectory[idx]->circle) {
                trajectory[idx]->circle = new SDL_Circle(trajectory[idx]->x, trajectory[idx]->y, trajectory[idx]->size);
            }
        }
    }

    void Physic_Circle::step() {

        if (SHOW_TRAJECTORY) {
            add_trajectory(this->x, this->y, this->radius);
            update_trajectory();
        }

        SDL_Circle::step(this->v_x, this->v_y);
        this->x += this->v_x;
        this->y += this->v_y;

        if (this->x + this->radius > WIDTH) {
            this->x = WIDTH - this->radius;
            this->v_x *= -1;
        }
        if (this->y + this->radius > HEIGHT) {
            this->y = HEIGHT - this->radius;
            this->v_y *= -1;
        }
        if (this->y - this->radius < 0) {
            this->y = this->radius;
            this->v_y *= -1;
        }
        if (this->x - this->radius < 0) {
            this->x = this->radius;
            this->v_x *= -1;
        }
    }

    Physic_Circle::Physic_Circle(int x, int y, int radius, int v_x, int v_y) : SDL_Circle(x, y, radius), v_x(v_x), v_y(v_y) {
        if (fabs(this->v_x) < 1) {
            std::cout << "velocity less than 1, the ball stay at x axis.";
        }
        if (fabs(this->v_y) < 1) {
            std::cout << "velocity less than 1, the ball stay at y axis.";
        }
    }

    Physic_Circle::~Physic_Circle() {}

    void main_loop() {
        SDL_Surface* surface = window_tool::create_window("BouncyBall", WIDTH, HEIGHT);

        Physic_Circle* p_circle = new Physic_Circle(200, 200, RADIUS, 10, 10);
        SDL_Rect* erase_rect = window_tool::create_rect(0, 0, WIDTH, HEIGHT);

        SDL_Event event;
        bool simulation_running = true;
        while(simulation_running) {
            Uint32 frame_start = SDL_GetTicks();
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    simulation_running = false;
                }

                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        simulation_running = false;
                    }
                }
            }
            window_tool::draw_rect(surface, erase_rect, color_background); // background
            p_circle->step(); // move circle
            if (SHOW_TRAJECTORY) {
                window_tool::draw_trajectory(surface, p_circle, TRAJECTORY_SIZE);
            }
            window_tool::draw_circle(surface, p_circle, white_color);
            p_circle->update_velocity();
            window_tool::update_window();
            // 固定帧数
            Uint32 frame_cost = SDL_GetTicks() - frame_start;
            if (FRAME_TARGET > frame_cost) {
                SDL_Delay(FRAME_TARGET - frame_cost);
            }
            std::cout << "Asset: " << window_tool::Game_Assets_Ptr.size() << std::endl;
        }
        window_tool::finish_window(surface);
    }


}