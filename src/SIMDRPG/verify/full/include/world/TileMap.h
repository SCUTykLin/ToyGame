#pragma once
#include "gfx/Surface.h"
#include "core/Types.h"

#include <string>
#include <vector>

namespace world {

using core::Rect;
using core::Pixel;

// 地图里 [ENTITY] 段的一行
struct EntitySpawn {
    std::string type;         // enemy_bandit / npc_elder / portal / boss_trex
    int x = 0, y = 0;         // 格子坐标（不是像素）

    // 只有 portal 用得上
    std::string target_map;
    int tx = 0, ty = 0;
};

class TileMap {
public:
    static constexpr int TS = 32;      // tile 边长（像素）

    std::string name, tileset, bg, weather;

    int w = 0, h = 0;                  // 以 tile 为单位
    int tpr = 16;                      // tiles per row：图集每行几个 tile

    std::vector<int>     ground;       // 每格的地形索引
    std::vector<uint8_t> solid;        // 1 = 挡住
    std::vector<uint8_t> decor;        // 每格的装饰物索引（0 = 空，i = PROPS[i-1]）
    std::vector<EntitySpawn> spawns;

    gfx::Surface image;                // 图集

    TileMap() = default;

    // image 是 Surface（禁拷贝），所以隐式移动会被抑制，必须显式声明。
    // 报错是 C2280「尝试引用已删除的函数」，看不出跟 Surface 有关。
    TileMap(const TileMap&) = delete;
    TileMap& operator=(const TileMap&) = delete;
    TileMap(TileMap&&) noexcept = default;
    TileMap& operator=(TileMap&&) noexcept = default;

    static TileMap load(const std::string& path);

    // ---------------------------------------------------------------
    // 编辑器接口
    //
    // 这四个函数是**纯逻辑**：不碰 SDL、不碰鼠标、不碰渲染。
    // 所以它们能被脚本驱动做回归测试（tests/edit_test.cpp），
    // GUI 只是套在外面的一层壳。
    //
    // 全部做越界保护 —— 鼠标拖到窗口外面是常态，不能崩。
    // ---------------------------------------------------------------
    void paint(int x, int y, int index) {
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        ground[size_t(y) * size_t(w) + size_t(x)] = index;
    }

    void set_solid(int x, int y, bool s) {
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        solid[size_t(y) * size_t(w) + size_t(x)] = s ? 1 : 0;
    }

    void toggle_solid(int x, int y) {
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        uint8_t& v = solid[size_t(y) * size_t(w) + size_t(x)];
        v = v ? 0 : 1;
    }

    // 填一片装饰物（编辑器里选区填充用）
    void fill_decor(int x0, int y0, int x1, int y1, int idx) {
        if (x0 > x1) { const int t = x0; x0 = x1; x1 = t; }
        if (y0 > y1) { const int t = y0; y0 = y1; y1 = t; }
        for (int y = y0; y <= y1; ++y)
            for (int x = x0; x <= x1; ++x)
                paint_decor(x, y, idx);
    }

    // 矩形填充（从按下拖到抬起，两角之间全填）
    void fill(int x0, int y0, int x1, int y1, int index);

    // 存回 .map。写出来的格式和 load 读的格式**完全对称**——
    // 保存后重新 load 必须得到一模一样的地图（edit_test 会验证这点）。
    bool save(const std::string& path) const;

    // 换图集（编辑器里按 Tab 在四季之间循环）。
    // 先找 terrain/，再找 backgrounds/；都找不到返回 false 且不动现有图。
    bool set_tileset(const std::string& file);

    // 图集里一共有几个 tile —— 用来 clamp 画笔索引，防止画到越界
    int tile_count() const {
        const int rows = (image.height() > 0) ? image.height() / TS : 0;
        return tpr * rows;
    }

    int at(int x, int y) const {
        return ground[size_t(y) * size_t(w) + size_t(x)];
    }

    // 装饰物索引。0 = 空格子。
    // 装饰层可能是空的（老地图没 [DECOR] 段），所以要先判空。
    int decor_at(int x, int y) const {
        if (decor.empty()) return 0;
        if (x < 0 || y < 0 || x >= w || y >= h) return 0;
        return decor[size_t(y) * size_t(w) + size_t(x)];
    }

    void paint_decor(int x, int y, int idx) {
        if (decor.empty()) return;
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        decor[size_t(y) * size_t(w) + size_t(x)] = uint8_t(idx);
    }

    bool is_solid(int x, int y) const {
        if (x < 0 || y < 0 || x >= w || y >= h) return true;   // 地图外一律当墙
        return solid[size_t(y) * size_t(w) + size_t(x)] != 0;
    }

    Rect tile_rect(int index) const {
        return { (index % tpr) * TS, (index / tpr) * TS, TS, TS };
    }

    int pixel_width()  const { return w * TS; }
    int pixel_height() const { return h * TS; }
};

} // namespace world
