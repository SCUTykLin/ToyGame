
#ifndef TOYPROJECTS_BOUNCYBALL_BOUNCY_BALL_H
#define TOYPROJECTS_BOUNCYBALL_BOUNCY_BALL_H

#include <SDL2/SDL.h>
#include "window_tool.h"

#define WIDTH 900
#define HEIGHT 600
#define GRAVITY 0
#define TRAJECTORY_SIZE 50
#define SHOW_TRAJECTORY 1
#define RADIUS 25

namespace bouncy {

    extern SDL_Surface* surface;

    namespace window_tool {
        struct SDL_Circle;
    }

    struct Physic_Circle : window_tool::SDL_Circle{
        int v_x;
        int v_y;

        struct Trajectory {
            int x;
            int y;
            int size;
            SDL_Circle* circle = nullptr;
            int alpha;
            double radius;
            Trajectory(int x, int y, int size) : x(x), y(y), size(size), alpha(TRAJECTORY_SIZE) {}
            void resetTrajectory(int x, int y, int size, int alpha) {
                this->x = x;
                this->y = y;
                this->size = size;
                this->alpha = alpha;

                int delta_x = x - circle->x;
                int delta_y = y - circle->y;

                circle->x += delta_x;
                circle->y += delta_y;

                radius = (static_cast<double>(alpha) / TRAJECTORY_SIZE) * RADIUS;

                for (int i = 0; i < circle->circle_primitives->size(); ++i) {
                    circle->circle_primitives->at(i)->x += delta_x;
                    circle->circle_primitives->at(i)->y += delta_y;
                }
            }
            ~Trajectory() {
                delete circle;
            }
        };

        Trajectory* trajectory[TRAJECTORY_SIZE];
        int trajectory_length = 0;
        int current_index = 0;

        Uint32 get_trajectory_color(SDL_PixelFormat* format, int alpha) {
            if (alpha > TRAJECTORY_SIZE) {
                alpha = TRAJECTORY_SIZE;
            }

            if (alpha < 0) {
                alpha = 0;
            }

            // 橙色
            constexpr Uint8 ORANGE_R = 255;
            constexpr Uint8 ORANGE_G = 165;
            constexpr Uint8 ORANGE_B = 0;

            // 背景色，假设是黑色
            constexpr Uint8 BG_R = 0;
            constexpr Uint8 BG_G = 0;
            constexpr Uint8 BG_B = 0;

            // alpha = 0：完全接近背景色
            // alpha = 9：完整橙色
            Uint8 r = BG_R + (ORANGE_R - BG_R) * alpha / TRAJECTORY_SIZE;
            Uint8 g = BG_G + (ORANGE_G - BG_G) * alpha / TRAJECTORY_SIZE;
            Uint8 b = BG_B + (ORANGE_B - BG_B) * alpha / TRAJECTORY_SIZE;

            return SDL_MapRGB(format, r, g, b);
        }

        Physic_Circle(int x, int y, int radius, int v_x, int v_y);
        ~Physic_Circle();
        void add_trajectory(int x, int y, int size);
        void delete_trajectory();
        void update_trajectory();
        void step();
        void update_velocity();
    };

    void start_bouncy();
    void main_loop();
}

#endif //TOYPROJECTS_BOUNCYBALL_BOUNCY_BALL_H