//
// Created by YangKai.Lin on 2026/7/9.
//

#include "fluid.h"

#include <iostream>
#include <ostream>
#include <algorithm>
#include "window_tool.h"

namespace fluid {
    void start_fluid() {
        main_loop();
    }

    Uint32 get_interpolated_color(Uint32 min, Uint32 max, double percentage) {
        unsigned char r1 = (min >> 16) & 0xFF;
        unsigned char r2 = (max >> 16) & 0xFF;
        unsigned char g1 = (min >> 8) & 0xFF;
        unsigned char g2 = (max >> 8) & 0xFF;
        unsigned char b1 = min & 0xFF;
        unsigned char b2 = max & 0xFF;
        double ratio = std::clamp(percentage, 0.0, 1.0);
        return (int)((r2 - r1) * ratio + r1) << 16 | (int)((g2 - g1) * ratio + g1) << 8 | (int)((b2 - b1) * ratio + b1);
    }

    void draw_cell(SDL_Surface* surface, Cell cell, int fill_cell) {
        int pixel_x = cell.x * CELL_SIZE;
        int pixel_y = cell.y * CELL_SIZE;
        SDL_Rect cell_rect = SDL_Rect{pixel_x, pixel_y, CELL_SIZE, CELL_SIZE};
        Uint32 color = BLACK_COLOR;
        SDL_FillRect(surface, &cell_rect, color);

        if (cell.type == WATER_TYPE) {
            int water_height = cell.fill_level > 1 ? CELL_SIZE : cell.fill_level * CELL_SIZE;
            int empty_height = CELL_SIZE - water_height;
            SDL_Rect water_rect = SDL_Rect{pixel_x, pixel_y + empty_height, CELL_SIZE, water_height};
            Uint32 interpolated_color = get_interpolated_color(BLUE_COLOR_MIN, BLUE_COLOR_MAX, cell.fill_level);
            if (cell.fill_level < 0.1) {
                interpolated_color = BLACK_COLOR;
            }
            if (fill_cell)
                SDL_FillRect(surface, &cell_rect, interpolated_color);
            else
                SDL_FillRect(surface, &water_rect, interpolated_color);
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
            draw_cell(surface, environment[i], 1);
        }
    }

    void simulation_phase_rule1(Cell environment[], double delta_time) {
        Cell environment_next[ROWS * COLUMNS];
        for (int i = 0; i < ROWS * COLUMNS; ++i) {
            environment_next[i] = environment[i];
        }
        // Rule 1: Water flows down
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLUMNS; ++j) {
                Cell source_cell = environment[j + COLUMNS * i];
                if (source_cell.type == WATER_TYPE && i < ROWS - 1) {
                    // How much liquid can flow into the destination_cell ?
                    Cell destination_cell = environment[j + COLUMNS * (i + 1)];
                    if (destination_cell.fill_level < source_cell.fill_level) {
                        double free_space_destination = 1 - destination_cell.fill_level;
                        if (free_space_destination >= source_cell.fill_level) {
                            environment_next[j + COLUMNS * i].fill_level = 0;
                            environment_next[j + COLUMNS * (i + 1)].fill_level += source_cell.fill_level;
                        }
                        else{
                            environment_next[j + COLUMNS * i].fill_level -= free_space_destination;
                            environment_next[j + COLUMNS * (i + 1)].fill_level = 1;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < ROWS * COLUMNS; ++i) {
            environment[i] = environment_next[i];
        }
    }

    void simulation_phase_rule2(Cell environment[]) {

        Cell environment_next[ROWS * COLUMNS];
        for (int i = 0; i < ROWS * COLUMNS; ++i) {
            environment_next[i] = environment[i];
        }

        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLUMNS; ++j) {
                Cell source_cell = environment[j + COLUMNS * i];
                // check if cell below is either full or solid
                int below_full_or_solid = 0;
                if (i+1 == ROWS || environment[j + COLUMNS * (i + 1)].fill_level >= environment[j + COLUMNS * i].fill_level || environment[j+COLUMNS*(i+1)].type == SOLID_TYPE) {
                    // Rule 2: water flowing left and right
                    if (source_cell.type == WATER_TYPE && j > 0) {
                        // How much liquid can flow to the left?
                        Cell destination_cell = environment[(j-1) + COLUMNS * i];
                        if (destination_cell.type == WATER_TYPE && destination_cell.fill_level < source_cell.fill_level) {
                            double delta_fill = source_cell.fill_level - destination_cell.fill_level;
                            environment_next[j + COLUMNS * i].fill_level -= delta_fill / 3;
                            environment_next[(j - 1) + COLUMNS * i].fill_level += delta_fill / 3;
                        }
                    }

                    if (source_cell.type == WATER_TYPE && j < COLUMNS - 1) {
                        // How much liquid can flow to the left?
                        Cell destination_cell = environment[(j+1) + COLUMNS * i];
                        if (destination_cell.type == WATER_TYPE && destination_cell.fill_level < source_cell.fill_level) {
                            double delta_fill = source_cell.fill_level - destination_cell.fill_level;
                            environment_next[j + COLUMNS * i].fill_level -= delta_fill / 3;
                            environment_next[(j + 1) + COLUMNS * i].fill_level += delta_fill / 3;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < ROWS * COLUMNS; ++i) {
            environment[i] = environment_next[i];
        }
    }

    // Rule 3: Pressurized cells can release fluid upwards.
    void simulation_phase_rule3(Cell environment[]) {

        Cell environment_next[ROWS * COLUMNS];
        for (int i = 0; i < ROWS * COLUMNS; ++i) {
            environment_next[i] = environment[i];
        }

        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLUMNS; ++j) {
                // Check if source cell fill level > 1
                // check if there is a water cell above into which fluid can be transferred
                Cell source_cell = environment[j + COLUMNS * i];
                if (source_cell.type == WATER_TYPE
                    && source_cell.fill_level > 1
                    && i > 0
                    && environment[j + COLUMNS * (i-1)].type == WATER_TYPE
                    && source_cell.fill_level > environment[j + COLUMNS * (i - 1)].fill_level) {
                    // cell is pressurized and water can flow up
                    double transfer_fill = (source_cell.fill_level - 1);
                    environment_next[j + COLUMNS * i].fill_level -= transfer_fill;
                    environment_next[j + COLUMNS * (i - 1)].fill_level += transfer_fill;
                }
            }
        }

        for (int i = 0; i < ROWS * COLUMNS; ++i) {
            environment[i] = environment_next[i];
        }
    }

    void simulation_step(Cell environment[], double delta_time) {
        simulation_phase_rule1(environment, delta_time);
        simulation_phase_rule2(environment);
        simulation_phase_rule3(environment);
    }

    // logic function for changing the data Structure
    void main_loop() {
        SDL_Surface* surface = window_tool::create_window("Liquid Simulation", SCREEN_WIDTH, SCREEN_HEIGHT);

        draw_grid(surface);
        Cell environment[ROWS * COLUMNS];
        initialize_environment(environment);

        SDL_Event event;
        bool simulation_running = true;
        int current_type = SOLID_TYPE;
        bool delete_mode = 0;

        Uint64 previous_time = SDL_GetPerformanceCounter();
        while (simulation_running) {
            Uint64 current_time = SDL_GetPerformanceCounter();
            double delta_time = static_cast<double>(current_time - previous_time) / static_cast<double>(SDL_GetPerformanceFrequency());

            previous_time = current_time;

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    simulation_running = false;
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

            // perform simulation stage
            simulation_step(environment, delta_time);

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
}