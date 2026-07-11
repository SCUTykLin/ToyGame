//
// Created by 15813 on 2026/7/11.
//

#include "Grid.h"

#include <stdexcept>
#include <algorithm>
fluid::Grid::Grid(int rows, int columns) : _rows(rows), _columns(columns), _fill_levels(rows * columns, 0.0f), _types(rows * columns, AIR_TYPE) {}

int fluid::Grid::index(int row, int column) const {
    row = std::max(std::min(row, _rows-1), 0);
    column = std::max(std::min(column, _columns-1), 0);
    return row * _columns + column;
}

float & fluid::Grid::fill_level(int row, int column) {
    return _fill_levels[index(row, column)];
}

const float fluid::Grid::fill_level(int row, int column) const {
    const auto temp = _fill_levels[index(row, column)];
    return temp;
}

int fluid::Grid::rows() const {
    return _rows;
}

int fluid::Grid::columns() const {
    return _columns;
}

bool fluid::Grid::inBounds(int row, int column) {
    return row >= 0 && column >= 0 && row < _rows && column < _columns;
}

std::uint8_t fluid::Grid::type(int row, int column) const {
    return _types[index(row, column)];
}

void fluid::Grid::set_type(int row, int column, const std::uint8_t type) {
    _types[index(row, column)] = type;
}

void fluid::Grid::set_fill_level(int row, int column, const float level) {
    _fill_levels[index(row, column)] = level;
}

void fluid::Grid::simulation_step() {
    simulation_rules_1();
    simulation_rules_2();
    simulation_rules_3();
}

void fluid::Grid::simulation_rules_1() {
    for (int row = rows() - 2; row >= 0; --row) {
        for (int column = columns() - 2; column >= 0; --column) {
            // 水从row流到row + 1
            if (type(row+1, column) == SOLID_TYPE || type(row, column) == SOLID_TYPE || type(row, column) == AIR_TYPE) { // 水留不下来 或 没有水要流下来
                continue;
            }
            float free_space = 1 - fill_level(row+1, column);
            if (free_space > 0) {
                float transfer = std::min(fill_level(row, column), free_space);
                set_fill_level(row, column, fill_level(row, column) - transfer);
                set_fill_level(row+1, column, fill_level(row + 1, column)+transfer);
            }

            if (fill_level(row, column) <= 0) {
                set_type(row, column, AIR_TYPE);
            }

            if (fill_level(row+1, column) > 0) {
                set_type(row+1, column, WATER_TYPE);
            }

        }
    }
}

bool fluid::Grid::is_supported(int row, int column) {
    constexpr float FULL_LEVEL = 0.999f;

    // 最后一行视为地板
    if (row == rows() - 1) {
        return true;
    }

    const auto below_type = type(row + 1, column);

    if (below_type == SOLID_TYPE) {
        return true;
    }

    // 下方水格已经基本满了，也可以认为有支撑
    return below_type == WATER_TYPE &&
           fill_level(row + 1, column) >= FULL_LEVEL;
}

void fluid::Grid::balance_horizontal_pair(int row, int column) {
    constexpr float EPSILON = 0.0001f;
    constexpr float LATERAL_RATE = 1.0f;

    const int left_column = column;
    const int right_column = column + 1;

    const auto left_type = type(row, left_column);
    const auto right_type = type(row, right_column);

    float left_level =
        fill_level(row, left_column);

    float right_level =
        fill_level(row, right_column);

    const float difference =
        left_level - right_level;

    if (std::abs(difference) <= EPSILON) {
        return;
    }

    if (difference > 0.0f) {
        // 左边水多，准备从左向右流
        if (left_type != WATER_TYPE ||
            !is_supported(row, left_column)) {
            return;
        }

        const float desired_transfer =
            difference * 0.5f * LATERAL_RATE;

        const float destination_space =
            1.0f - right_level;

        const float transfer = std::min({
            desired_transfer,
            left_level,
            destination_space
        });

        fill_level(row, left_column) -= transfer;
        fill_level(row, right_column) += transfer;
    } else {
        // 右边水多，准备从右向左流
        if (right_type != WATER_TYPE ||
            !is_supported(row, right_column)) {
            return;
        }

        const float desired_transfer =
            (-difference) * 0.5f * LATERAL_RATE;

        const float destination_space =
            1.0f - left_level;

        const float transfer = std::min({
            desired_transfer,
            right_level,
            destination_space
        });

        fill_level(row, right_column) -= transfer;
        fill_level(row, left_column) += transfer;
    }

    // 根据最终水量更新类型
    if (fill_level(row, left_column) > EPSILON) {
        set_type(row, left_column, WATER_TYPE);
    } else {
        fill_level(row, left_column) = 0.0f;
        set_type(row, left_column, AIR_TYPE);
    }

    if (fill_level(row, right_column) > EPSILON) {
        set_type(row, right_column, WATER_TYPE);
    } else {
        fill_level(row, right_column) = 0.0f;
        set_type(row, right_column, AIR_TYPE);
    }

}

void fluid::Grid::process_horizontal_pairs(int start_index) {
    for (int row = rows() - 2; row >= 0; --row) {
        for (int column = start_index; column + 1 < columns(); column+=2) {
            if (type(row, column) != SOLID_TYPE &&
                type(row, column + 1) != SOLID_TYPE &&
                ((type(row, column) == WATER_TYPE && fill_level(row, column) > 0) || (type(row, column+1) == WATER_TYPE && fill_level(row, column+1) > 0))) {
                balance_horizontal_pair(row, column);
            }
        }
    }
}

void fluid::Grid::simulation_rules_2() {
    static int start_index = 0;

    for (int step = 0; step < HORIZONTAL_SUBSTEPS; ++step) {
        process_horizontal_pairs(start_index);
        start_index = 1 - start_index;
    }
}

void fluid::Grid::simulation_rules_3() {
    constexpr float CAPACITY = 1.0f;

    for (int row = 1; row < rows(); ++row) {
        for (int column = 0; column < columns(); ++column) {
            if (type(row, column) != WATER_TYPE ||
                type(row - 1, column) == SOLID_TYPE) {
                continue;
                }

            const float excess =
                fill_level(row, column) - CAPACITY;

            if (excess <= 0.0f) {
                continue;
            }

            fill_level(row, column) -= excess;
            fill_level(row - 1, column) += excess;

            set_type(row - 1, column, WATER_TYPE);
        }
    }
}