//
// Created by YangKai.Lin on 2026/7/9.
//

#include "fluid.h"

#include <iostream>
#include <ostream>

#include "window_tool.h"

namespace fluid {
    void start_fluid() {
        main_loop();
    }
    void draw_cell(SDL_Surface* surface, Cell cell) {
        int pixel_x = cell.x * CELL_SIZE;
        int pixel_y = cell.y * CELL_SIZE;
        SDL_Rect cell_rect = SDL_Rect{pixel_x, pixel_y, CELL_SIZE, CELL_SIZE};
        Uint32 color = WHITE_COLOR;
        if (cell.type == WATER_TYPE) {
            color = BLUE_COLOR;
        }
        SDL_FillRect(surface, &cell_rect, color);
    }

    void initialize_environment(Cell enviroment[]) {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0;j < COLUMNS; ++j) {
                enviroment[j + COLUMNS * i] = Cell{WATER_TYPE, 0, j, i};
            }
        }
    }

    // logic function for changing the data Structure
    void main_loop() {
        SDL_Surface* surface = window_tool::create_window("Liquid Simulation", SCREEN_WIDTH, SCREEN_HEIGHT);

        draw_grid(surface);
        struct Cell environment[ROWS * COLUMNS];

        SDL_Event event;
        bool simulation_running = true;
        int current_type = SOLID_TYPE;
        while (simulation_running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    simulation_running = false;
                }

                if (event.type == SDL_MOUSEMOTION) {
                    if (event.motion.state != 0) {
                        int cell_x = event.motion.x / CELL_SIZE;
                        int cell_y = event.motion.y / CELL_SIZE;
                        draw_cell(surface, Cell(current_type, 0, cell_x, cell_y));
                    }
                }

                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        current_type = !current_type;
                    }
                }
            }
            window_tool::update_window();
        }
    }

    void draw_grid(SDL_Surface* surface) {
        for (int i = 0; i <COLUMNS; i++) {
            SDL_Rect column = SDL_Rect{i * CELL_SIZE, 0, LINE_WIDTH, SCREEN_HEIGHT};
            SDL_FillRect(surface, &column, GRAY_COLOR);
        }
        for (int j = 0; j < ROWS; j++) {
            SDL_Rect row = SDL_Rect{0, j * CELL_SIZE, SCREEN_WIDTH, LINE_WIDTH};
            SDL_FillRect(surface, &row, GRAY_COLOR);
        }
    }

    // draw the canvas
    void draw() {

    }
}