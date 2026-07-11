//
// Created by YangKai.Lin on 2026/7/9.
//

#ifndef TOYPROJECTS_WINDOW_TOOL_H
#define TOYPROJECTS_WINDOW_TOOL_H

#include <string>
#include <SDL2/SDL.h>
#include "Grid/Grid.h"
#include <algorithm>
#include <ratio>

namespace fluid::window_tool {
    extern SDL_Window* window;
    extern SDL_Rect* canvas;
    SDL_Surface* create_window(const std::string& title, int width, int height);
    void clear_window(SDL_Surface* surface, Uint32 color=0x00000000);
    void update_window();
    void destroy_window();

    Uint32 get_interpolated_color(Uint32 min, Uint32 max, double percentage);

    template <typename T>
    void draw_cell(SDL_Surface* surface, const T& grid, int row, int column) {
        if (grid.type(row, column) == SOLID_TYPE) {
            SDL_Rect cell_rect = SDL_Rect{column * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_FillRect(surface, &cell_rect, WHITE_COLOR);
        }
        if (grid.type(row, column) == WATER_TYPE) {
            SDL_Rect cell_rect = SDL_Rect{column * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_FillRect(surface, &cell_rect, get_interpolated_color(BLUE_COLOR_MIN, BLUE_COLOR_MAX, grid.fill_level(row, column)));
        }

        if (grid.type(row, column) == AIR_TYPE) {
            SDL_Rect cell_rect = SDL_Rect{column * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_FillRect(surface, &cell_rect, BLUE_COLOR_MIN);
        }
    }

    template <typename T>
    void draw(SDL_Surface* surface, const T& grid) {
        for (int i = 0; i < grid.rows(); i++) {
            for (int j = 0; j < grid.columns(); j++) {
                if (grid.type(i, j) != AIR_TYPE)
                    draw_cell<T>(surface, grid, i, j);
            }
        }
    }

    template <
        int Rows,
        int Columns,
        int CellSize,
        int ScreenWidth,
        int ScreenHeight,
        int LineWidth>
    void draw_grid(SDL_Surface* surface, Uint32 color) {
        for (int i = 0; i < Columns; i++) {
            SDL_Rect column = SDL_Rect{i * CellSize, 0, LineWidth, ScreenHeight};
            SDL_FillRect(surface, &column, color);
        }
        for (int j = 0; j < Rows; j++) {
            SDL_Rect row = SDL_Rect{0, j * CellSize, ScreenWidth, LineWidth};
            SDL_FillRect(surface, &row, color);
        }
    }

}


#endif //TOYPROJECTS_WINDOW_TOOL_H