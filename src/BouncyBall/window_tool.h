//
// Created by YangKai.Lin on 2026/7/7.
//

#ifndef TOYPROJECTS_BOUNCYBALL_WINDOW_TOOL_H
#define TOYPROJECTS_BOUNCYBALL_WINDOW_TOOL_H
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <SDL2/SDL.h>

#include "bouncy_ball.h"

namespace bouncy {
    namespace window_tool {
        extern SDL_Window* window;
        extern std::vector<std::function<void()>> Game_Assets_Delete;
        extern std::vector<void*> Game_Assets_Ptr;

        typedef struct SDL_Circle {
            int x;
            int y;
            int radius;
            std::vector<SDL_Rect*>* circle_primitives;

            float circle_sdf(int coordinate_x, int coordinate_y) {
                return std::pow(coordinate_x - x, 2) + std::pow(coordinate_y - y, 2) - std::pow(this->radius, 2);
            }
            SDL_Circle(int x, int y, int radius);

            ~SDL_Circle();
        }SDL_Circle;

        SDL_Surface* create_window(std::string title, int width, int height);

        SDL_Circle* create_cirle(int x, int y, int radius);

        SDL_Rect* create_rect(int x, int y, int w, int h);

        void delete_assets();

        template <typename T>
        concept hasSDL_Circle = requires(T){std::declval<T>().sdl_circle;};

        template <typename T>
        concept hasCircle_Primitives = requires(T){std::declval<T>().circle_primitives;};

        template <typename T>
        void delete_assets(T* asset) {
            if (asset == nullptr) return;

            auto ptr_it = std::find(Game_Assets_Ptr.begin(), Game_Assets_Ptr.end(),
                                    static_cast<void*>(asset));
            const bool exists = (ptr_it != Game_Assets_Ptr.end());
            const int idx = static_cast<int>(ptr_it - Game_Assets_Ptr.begin());

            // 自动检测：有对应成员就自动递归删除
            if constexpr (hasCircle_Primitives<T>) {
                for (auto* sub : *asset->circle_primitives) {
                    delete_assets(sub);
                }
            }

            if constexpr (hasSDL_Circle<T>) {
                delete_assets(asset->sdl_circle);
            }

            if (exists) {
                Game_Assets_Delete[idx]();
                Game_Assets_Ptr.erase(ptr_it);
                Game_Assets_Delete.erase(Game_Assets_Delete.begin() + idx);
            }
        }

        template <typename T, typename U = void>
        struct type_traits_circle_ptr {
            using type = T*;
        };

        template <typename T>
        struct type_traits_circle_ptr<T, std::void_t<decltype(std::declval<T>().sdl_circle)>> {
            using type = decltype(std::declval<T>().sdl_circle);
        };

        template <typename T>
        void draw_circle(SDL_Surface* surface, T* circle, Uint32 color) {
            if (circle) {
                using rawT = std::remove_cv_t<std::remove_reference_t<T>>;
                typename type_traits_circle_ptr<rawT>::type _temp_circle = nullptr;
                if (std::is_same_v<rawT, SDL_Circle>) {
                    _temp_circle = reinterpret_cast<SDL_Circle*>(circle);
                }
                else if (hasSDL_Circle<T>) {
                    _temp_circle = circle->sdl_circle;
                }
                for (size_t i = 0; i < _temp_circle->circle_primitives->size(); ++i) {
                    SDL_FillRect(surface, _temp_circle->circle_primitives->at(i), color);
                }
            }
        }

        void draw_rect(SDL_Surface* surface, SDL_Rect* rect, Uint32 color);

        void move_rect(SDL_Surface* surface, SDL_Rect* rect);

        void update_window();

    }
}

#endif //TOYPROJECTS_BOUNCYBALL_WINDOW_TOOL_H