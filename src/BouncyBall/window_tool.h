
#ifndef TOYPROJECTS_BOUNCYBALL_WINDOW_TOOL_H
#define TOYPROJECTS_BOUNCYBALL_WINDOW_TOOL_H
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <functional>
#include <SDL2/SDL.h>

namespace bouncy::window_tool {
        extern SDL_Window* window;
        extern std::vector<std::function<void()>> Game_Assets_Delete;
        extern std::vector<void*> Game_Assets_Ptr;

        struct SDL_Circle {
            int x;
            int y;
            int radius;
            std::vector<SDL_Rect*>* circle_primitives;

            float circle_sdf(int coordinate_x, int coordinate_y) {
                return std::pow(coordinate_x - x, 2) + std::pow(coordinate_y - y, 2) - std::pow(this->radius, 2);
            }
            void step(int vx, int vy);
            SDL_Circle(int x, int y, int radius);
            void reset_circle_primitives(double size_perce);
            ~SDL_Circle();
        };

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

        template <typename T, bool U=false>
        void draw_circle(SDL_Surface* surface, T* circle, Uint32 color, std::optional<double> radius = std::nullopt) {
            if (circle) {
                if constexpr (U) {
                    if constexpr (static_cast<bool>(sizeof(decltype(std::declval<T>().v_x + std::declval<T>().v_y)))) {
                        // 有速度，才能退回上一帧擦除掉
                        for (size_t i = 0; i < circle->circle_primitives->size(); ++i) {
                            circle->circle_primitives->at(i)->x -= circle->v_x;
                            circle->circle_primitives->at(i)->y -= circle->v_y;
                            SDL_FillRect(surface, circle->circle_primitives->at(i), 0x00000000);
                            circle->circle_primitives->at(i)->x += circle->v_x;
                            circle->circle_primitives->at(i)->y += circle->v_y;
                        }
                    }
                }
                const bool use_radius = radius.has_value();
                const double r2 = use_radius ? (*radius) : 0.0;
                for (size_t i = 0; i < circle->circle_primitives->size(); ++i) {
                    if (use_radius && std::pow(circle->circle_primitives->at(i)->x - circle->x, 2) + std::pow(circle->circle_primitives->at(i)->y - circle->y, 2) - std::pow(r2, 2) > 0){
                        continue;
                    }
                    SDL_FillRect(surface, circle->circle_primitives->at(i), color);
                }
            }
        }

        template <typename T>
        void draw_trajectory(SDL_Surface* surface, T* physic_circle,  const size_t N) {
            using no_pointer_T = std::remove_pointer_t<decltype(physic_circle->trajectory[0]->circle)>;
            for (int idx = (physic_circle->current_index - 1 + N) % N, i = 0; i < physic_circle->trajectory_length; ++i, idx = (idx - 1 + N) % N) {
                draw_circle<no_pointer_T, false>(
                    surface,
                    physic_circle->trajectory[idx]->circle,
                    physic_circle->get_trajectory_color(surface->format, physic_circle->trajectory[idx]->alpha),
                    physic_circle->trajectory[idx]->radius
                    );
            }
        }
        void draw_rect(SDL_Surface* surface, SDL_Rect* rect, Uint32 color);

        void move_rect(SDL_Surface* surface, SDL_Rect* rect);

        void update_window();

        void finish_window(SDL_Surface* surface);

}

#endif //TOYPROJECTS_BOUNCYBALL_WINDOW_TOOL_H