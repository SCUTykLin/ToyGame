//
// Created by YangKai.Lin on 2026/7/9.
//

#ifndef TOYPROJECTS_FLUID_H
#define TOYPROJECTS_FLUID_H

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define WHITE_COLOR 0xffffffff
#define BLACK_COLOR 0x00000000
#define BLUE_COLOR_MAX 0x34c3eb
#define BLUE_COLOR_MIN 0x001eff
#define GRAY_COLOR 0x1f1f1f1f
#define CELL_SIZE 8
#define LINE_WIDTH 0
#define COLUMNS SCREEN_WIDTH/CELL_SIZE
#define ROWS SCREEN_HEIGHT/CELL_SIZE
#define WATER_TYPE 0
#define SOLID_TYPE  1
#define AIR_TYPE 2

#include <SDL2/SDL.h>

namespace fluid {
    constexpr double DOWN_FLOW_RATE = 0.1;
    constexpr double EPSILON = 0.0001;
    struct Cell {
        int type;
        double fill_level; // 0 to 1
        int x;
        int y;
        int flowing_down; // flag set by simulation routine.
        Cell(){}
        Cell(int type, double fill_level, int x, int y) : type(type), fill_level(fill_level), x(x), y(y){}
    };

    void start_fluid();
    void main_loop();
    void draw();
    void draw_grid(SDL_Surface* surface);
    void draw_cell(SDL_Surface* surface, Cell cell, int fill_cell);
}

#endif //TOYPROJECTS_FLUID_H