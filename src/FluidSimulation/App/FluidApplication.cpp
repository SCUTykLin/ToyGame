//
// Created by 15813 on 2026/7/11.
//

#include "FluidApplication.h"

#include <stdexcept>

#include "../Grid/Grid.h"
#include "../window_tool.h"
#include <iostream>

namespace fluid {

    void FluidApplication::run() {
        SDL_Surface* surface = window_tool::create_window("Liquid Simulation", SCREEN_WIDTH, SCREEN_HEIGHT);
        window_tool::draw_grid<ROWS, COLUMNS, CELL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, LINE_WIDTH>(surface, GRAY_COLOR);
        while (_running) {
            window_tool::clear_window(surface);
            process_events();
            _grid.simulation_step();
            window_tool::draw(surface, _grid);
            window_tool::update_window();
        }
        window_tool::destroy_window();
    }

    void FluidApplication::process_events() {
        static std::uint8_t current_type = SOLID_TYPE;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                _running = false;
            }

            if (event.type == SDL_MOUSEMOTION) {
                if (event.motion.state != 0) {
                    handle_mouse(event.motion.x, event.motion.y, current_type);
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    current_type = !current_type;
                }
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    current_type = AIR_TYPE;
                }
            }
        }
    }

    void FluidApplication::handle_mouse(int x, int y, std::uint8_t type) {
        int cell_column = x / CELL_SIZE;
        int cell_row = y / CELL_SIZE;
        if (type == WATER_TYPE || type == SOLID_TYPE) {
            if (_grid.type(cell_row, cell_column) == AIR_TYPE)
                _grid.set_fill_level(cell_row, cell_column, 1);
            else {
                _grid.fill_level(cell_row, cell_column) = _grid.fill_level(cell_row, cell_column) + 1.0f;
            }
        }else if (type == AIR_TYPE) {
            _grid.set_fill_level(cell_row, cell_column, 0);
        }else
            throw std::runtime_error("Unknown type");
        _grid.set_type(cell_row, cell_column, type);
    }
}