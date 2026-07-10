//
// Created by 15813 on 2026/7/11.
//

#include "FluidApplication.h"
namespace fluid {

    void FluidApplication::run() {

    }

    void FluidApplication::process_events() {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                _running = false;
            }

            if (event.type == SDL_MOUSEMOTION) {
                if (event.motion.state != 0) {
                    int cell_x = event.motion.x / CELL_SIZE; // x表示横轴
                    int cell_y = event.motion.y / CELL_SIZE; // y表示纵轴
                    double fill_level;
                    Cell cell;
                    if (delete_mode) {
                        current_type = WATER_TYPE;
                        fill_level = 0;
                        cell = Cell(current_type, fill_level, cell_x, cell_y);
                    }else {
                        fill_level = environment[cell_x + COLUMNS * cell_y].fill_level + 0.5;
                        cell = Cell(current_type, fill_level, cell_x, cell_y);
                    }
                    environment[cell_x + COLUMNS * cell_y] = cell;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    current_type = !current_type;
                }
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    delete_mode = !delete_mode;
                }
            }
        }
    }

    void FluidApplication::handle_mouse(int x, int y) {
        int cell_x = x / CELL_SIZE;
        int cell_y = y / CELL_SIZE;
    }

    void FluidApplication::update() {

    }

    void FluidApplication::render() {

    }
}