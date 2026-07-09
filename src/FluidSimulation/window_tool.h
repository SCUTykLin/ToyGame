//
// Created by YangKai.Lin on 2026/7/9.
//

#ifndef TOYPROJECTS_WINDOW_TOOL_H
#define TOYPROJECTS_WINDOW_TOOL_H

#include <string>
#include <SDL2/SDL.h>

namespace fluid::window_tool {
    extern SDL_Window* window;
    extern SDL_Rect* canvas;
    SDL_Surface* create_window(const std::string& title, int width, int height);
    void clear_window();
    void update_window();

}


#endif //TOYPROJECTS_WINDOW_TOOL_H