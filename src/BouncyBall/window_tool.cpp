//
// Created by YangKai.Lin on 2026/7/7.
//

#include "window_tool.h"
namespace bouncy {
    namespace window_tool {
        SDL_Window* window = nullptr;
        std::vector<std::function<void()>> Game_Assets_Delete;
        std::vector<void*> Game_Assets_Ptr;

        void SDL_Circle::step(int vx, int vy) {
            for (SDL_Rect* r : *circle_primitives) {
                r->x += vx;
                r->y += vy;
            }
        }

        SDL_Circle::SDL_Circle(int x, int y, int radius) : x(x), y(y), radius(radius) {

            this->circle_primitives = new std::vector<SDL_Rect*>();
            int low_x = this->x - this->radius;
            int low_y = this->y - this->radius;
            int high_x = this->x + this->radius;
            int high_y = this->y + this->radius;

            for (int ix = low_x; ix <= high_x; ix++) {
                for (int iy = low_y; iy <= high_y; iy++) {
                    if (circle_sdf(ix, iy) <= 0) {
                        auto _temp = create_rect(ix, iy, 1, 1);
                        circle_primitives->emplace_back(_temp);
                    }
                }
            }
        }

        SDL_Circle::~SDL_Circle() {
            for (size_t i = 0; i < this->circle_primitives->size(); i++) {
                delete_assets(this->circle_primitives->at(i));
            }
            delete this->circle_primitives;
        }

        SDL_Surface* create_window(std::string title, int width, int height){
            SDL_InitSubSystem(SDL_INIT_VIDEO);
            window = SDL_CreateWindow(title.c_str(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                width, height, 0);
            SDL_Surface* surface = SDL_GetWindowSurface(window);
            return surface;
        }
        SDL_Circle* create_cirle(int x, int y, int radius) {
            SDL_Circle* circle = new SDL_Circle(x, y, radius);
            Game_Assets_Ptr.emplace_back(static_cast<void*>(circle));
            Game_Assets_Delete.emplace_back(
                [circle]()->void {
                    if(circle) {
                        delete circle;
                    }
                }
            );
            return circle;
        }
        SDL_Rect* create_rect(int x, int y, int w, int h) {
            SDL_Rect* rect = new SDL_Rect(x, y, w, h);
            Game_Assets_Ptr.emplace_back(reinterpret_cast<void*>(rect));
            Game_Assets_Delete.emplace_back(
                [rect]()->void {
                    if(rect) {
                        delete rect;
                    }
                }
                );
            return rect;
        }

        void finish_window(SDL_Surface* surface) {
            SDL_FreeSurface(surface);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        void delete_assets() {
            for (int i = 0; i < Game_Assets_Delete.size(); i++) {
                Game_Assets_Delete[i]();
            }
            Game_Assets_Delete.clear();
            Game_Assets_Ptr.clear();
        }

        void draw_rect(SDL_Surface* surface, SDL_Rect* rect, Uint32 color) {
            if (rect) {
                SDL_FillRect(surface, rect, color);
            }
        }

        void move_rect(SDL_Surface* surface, SDL_Rect* rect) {

        }

        void update_window() {
            SDL_UpdateWindowSurface(window);
        }
    }
}