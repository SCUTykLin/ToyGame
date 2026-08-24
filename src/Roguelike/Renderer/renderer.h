//
// Created by 15813 on 2026/7/12.
//

#ifndef TOYPROJECTS_ROGUELIKE_RENDERER_H
#define TOYPROJECTS_ROGUELIKE_RENDERER_H

#include "SDL2/SDL.h"
#include <iostream>
#include <mutex>

template<typename T, typename=void>
struct can_iterate : std::false_type{};

template<typename T>
struct can_iterate<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type{};

namespace roguelike {

    class Renderer {
    public:
        void create_window(const char* title, int xpos, int ypos, int width, int height, bool fullscreen, bool& isRunning);

        SDL_Renderer* get_window_renderer();

        SDL_Window* get_window();

        template <typename... GameObjects>
        void render(GameObjects&&... gameObjects) {
            SDL_RenderClear(renderer);

            ([&]()->void {
                if constexpr (can_iterate<GameObjects>::value){
                    for (auto& p : std::forward<GameObjects>(gameObjects))
                    {
                        p->draw();
                    }
                }
                else
                    std::forward<GameObjects>(gameObjects).draw();
            }(), ...);

            SDL_RenderPresent(renderer);
        }

        void destroy_window();

    private:
        static SDL_Window *window;
        static SDL_Renderer *renderer;
        static std::once_flag init_flag;
    };

}

#endif //TOYPROJECTS_ROGUELIKE_RENDERER_H
