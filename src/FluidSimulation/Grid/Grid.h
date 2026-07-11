//
// Created by 15813 on 2026/7/11.
//

#ifndef TOYPROJECTS_GRID_H
#define TOYPROJECTS_GRID_H

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define WHITE_COLOR 0xffffffff
#define BLACK_COLOR 0x00000000
#define BLUE_COLOR_MAX 0x34c3eb
#define BLUE_COLOR_MIN 0x001eff
#define GRAY_COLOR 0x1f1f1f1f
#define CELL_SIZE 5
#define LINE_WIDTH 2
#define COLUMNS SCREEN_WIDTH / CELL_SIZE
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define WATER_TYPE 0
#define SOLID_TYPE  1
#define AIR_TYPE   2

#define HORIZONTAL_SUBSTEPS 5 // substeps of rule2

#include <vector>

namespace fluid {
    class Grid {
    public:
        Grid() = delete;
        Grid(int rows, int columns);
        int index(int row, int column) const;
        float& fill_level(int row, int column);
        const float fill_level(int row, int column) const;
        int rows() const;
        int columns() const;
        [[nodiscard]]
        bool inBounds(int row, int column);
        std::uint8_t type(int row, int column) const;
        void set_type(int row, int column, const std::uint8_t type);
        void set_fill_level(int row, int column, const float level);
        bool is_supported(int row, int column);
        void simulation_step();
        void simulation_rules_1();
        void simulation_rules_2();
        void simulation_rules_3();

        void process_horizontal_pairs(int start_index);
        void balance_horizontal_pair(int row, int column);

    private:
        const int _rows;
        const int _columns;
        std::vector<float> _fill_levels;
        std::vector<std::uint8_t> _types;
    };
}
#endif //TOYPROJECTS_GRID_H
