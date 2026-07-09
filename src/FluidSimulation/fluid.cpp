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
        Uint32 color = BLACK_COLOR;
        SDL_FillRect(surface, &cell_rect, color);

        if (cell.type == WATER_TYPE) {
            int water_height = cell.fill_level * CELL_SIZE;
            int empty_height = CELL_SIZE - water_height;
            SDL_Rect water_rect = SDL_Rect{pixel_x, pixel_y + empty_height, CELL_SIZE, water_height};
            SDL_FillRect(surface, &water_rect, BLUE_COLOR);
        }
        if (cell.type == SOLID_TYPE){
            SDL_FillRect(surface, &cell_rect, WHITE_COLOR);
        }
    }

    void initialize_environment(Cell enviroment[]) {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0;j < COLUMNS; ++j) {
                enviroment[j + COLUMNS * i] = Cell{WATER_TYPE, 0, j, i};
            }
        }
    }

    void draw_environment(SDL_Surface* surface, Cell environment[]){
        for (int i = 0; i < ROWS * COLUMNS; ++i){
            draw_cell(surface, environment[i]);
        }
    }

    // logic function for changing the data Structure
    void main_loop() {
        SDL_Surface* surface = window_tool::create_window("Liquid Simulation", SCREEN_WIDTH, SCREEN_HEIGHT);

        draw_grid(surface);
        struct Cell environment[ROWS * COLUMNS];
        initialize_environment(environment);

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
                        if (cell_y + COLUMNS * cell_x < ROWS * COLUMNS && cell_y + COLUMNS * cell_x > 0) {
                            Cell cell = Cell(current_type, 0, cell_x, cell_y);
                            environment[cell_y + COLUMNS * cell_x] = cell;
                        }
                    }
                }

                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        current_type = !current_type;
                    }
                }
            }
            draw_environment(surface, environment);
            draw_grid(surface);
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