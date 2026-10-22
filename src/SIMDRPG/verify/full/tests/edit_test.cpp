// 地图编辑器的回归测试（不属于讲义，是仓库的验证程序）
//
// 编辑器有两半：TileMap 上的纯逻辑（paint/set_solid/save）
// 和 Game 里的鼠标外壳。GUI 没法脚本化，但纯逻辑可以。
//
// 这里验证四件事：
//   1. 越界操作不崩、不改数据
//   2. 画上去的确实改了
//   3. save → load 往返完全一致（包括实体、bg、weather）
//   4. 换图集会正确报告，找不到图集返回 false
//
// 构建（在 verify/full 目录，先编译过一次参考实现）：
//   cl /utf-8 /std:c++20 /DSDL_MAIN_HANDLED /EHsc /Iinclude ^
//      tests/edit_test.cpp src/world/TileMap.cpp src/gfx/Surface.cpp ^
//      src/gfx/Atlas.cpp /Fe:edit_test.exe /link <SDL2 的两个 .lib>
//
// 运行（必须在仓库根目录）：
//   ./verify/full/tests/edit_test.exe

#include "world/TileMap.h"

#include <cstdio>
#include <cstdlib>
#include <vector>

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);   // 崩溃时也要看到输出
    std::printf("[0] start\n");
    int fails = 0;
    auto check = [&](bool ok, const char* what) {
        if (!ok) { std::printf("  [FAIL] %s\n", what); ++fails; }
    };

    std::srand(12345);          // 固定种子，失败可复现

    // ---------------------------------------------------------------
    // 1) 加载原始地图
    // ---------------------------------------------------------------
    world::TileMap m = world::TileMap::load("assets/maps/village.map");
    const int W = m.w, H = m.h;
    std::printf("[1] 加载 %s  %dx%d  tpr=%d  实体 %zu\n",
                m.name.c_str(), W, H, m.tpr, m.spawns.size());

    const std::vector<int>     g0 = m.ground;
    const std::vector<uint8_t> s0 = m.solid;

    // ---------------------------------------------------------------
    // 2) 越界保护：鼠标拖到窗口外是常态
    // ---------------------------------------------------------------
    m.paint(-1, -1, 999);
    m.paint(W + 5, 0, 999);
    m.paint(0, H + 5, 999);
    m.set_solid(-3, -3, true);
    m.toggle_solid(W, H);
    check(m.ground == g0, "越界 paint 改动了数据");
    check(m.solid == s0, "越界 set_solid 改动了数据");
    std::printf("[2] 越界调用全部被挡住\n");

    // ---------------------------------------------------------------
    // 3) 画 500 笔
    // ---------------------------------------------------------------
    for (int i = 0; i < 500; ++i) {
        const int x = std::rand() % W;
        const int y = std::rand() % H;
        const int v = 20 + (std::rand() % 100);
        m.paint(x, y, v);
    }
    int changed = 0;
    for (int i = 0; i < W * H; ++i)
        if (m.ground[size_t(i)] != g0[size_t(i)]) ++changed;
    std::printf("[3] 画 500 笔，实际改动 %d 格\n", changed);
    check(changed > 300, "改动格数太少，paint 没生效");

    // ---------------------------------------------------------------
    // 4) 刷碰撞
    // ---------------------------------------------------------------
    for (int i = 0; i < 200; ++i)
        m.set_solid(std::rand() % W, std::rand() % H, true);

    int solid_added = 0;
    for (int i = 0; i < W * H; ++i)
        if (m.solid[size_t(i)] && !s0[size_t(i)]) ++solid_added;
    std::printf("[4] 刷 200 格碰撞，实际新增 %d\n", solid_added);
    check(solid_added > 100, "碰撞没刷上");

    // toggle 应该能翻回去
    const int tx = 5, ty = 5;
    const bool was = m.is_solid(tx, ty);
    m.toggle_solid(tx, ty);
    check(m.is_solid(tx, ty) != was, "toggle_solid 没翻转");
    m.toggle_solid(tx, ty);
    check(m.is_solid(tx, ty) == was, "toggle_solid 翻两次没回到原值");

    // ---------------------------------------------------------------
    // 5) 保存 → 重载 → 逐格比对
    // ---------------------------------------------------------------
    const char* tmp = "_edit_test.map";
    const bool saved = m.save(tmp);
    check(saved, "save 返回 false");

    if (saved) {
        world::TileMap m2 = world::TileMap::load(tmp);

        check(m2.w == W && m2.h == H,        "宽高不一致");
        check(m2.ground == m.ground,          "ground 往返不一致");
        check(m2.solid == m.solid,            "solid 往返不一致");
        check(m2.tileset == m.tileset,        "tileset 不一致");
        check(m2.bg == m.bg,                  "bg 不一致（带空格的文件名最容易坏）");
        check(m2.weather == m.weather,        "weather 不一致");
        check(m2.spawns.size() == m.spawns.size(), "实体数量不一致");

        for (size_t i = 0; i < m.spawns.size() && i < m2.spawns.size(); ++i) {
            check(m2.spawns[i].type == m.spawns[i].type, "实体 type 不一致");
            check(m2.spawns[i].x == m.spawns[i].x &&
                  m2.spawns[i].y == m.spawns[i].y,       "实体坐标不一致");
            check(m2.spawns[i].target_map == m.spawns[i].target_map,
                                                          "传送门目标不一致");
        }
        std::printf("[5] 保存→重载：%d 格 ground / %d 格 solid / %zu 个实体 全部一致\n",
                    W * H, W * H, m.spawns.size());
    }

    // ---------------------------------------------------------------
    // 6) 换图集（编辑器按 Tab 走的就是这条路）
    // ---------------------------------------------------------------
    {
        world::TileMap m3 = world::TileMap::load("assets/maps/rift.map");
        const int before = m3.tile_count();
        const int tpr_before = m3.tpr;

        if (m3.set_tileset("terrain_summer.png")) {
            std::printf("[6] 换图集：%d 个 tile (tpr=%d) → %d 个 (tpr=%d)\n",
                        before, tpr_before, m3.tile_count(), m3.tpr);

            // 换完之后旧索引可能越界 —— 编辑器的 Tab 处理会 clamp，
            // 这里验证 clamp 的必要性（真的会越界就说明 clamp 不是白写的）
            const int last = m3.tile_count() - 1;
            int out_of_range = 0;
            for (int g : m3.ground)
                if (g > last) ++out_of_range;
            std::printf("    换图集后有 %d 格索引越界（编辑器会自动夹到 %d）\n",
                        out_of_range, last);

            check(m3.tileset == "terrain_summer.png", "tileset 名没更新");
            check(m3.tile_rect(0).w == world::TileMap::TS, "tile_rect 尺寸不对");
        } else {
            std::printf("  [FAIL] set_tileset 对存在的图集返回了 false\n");
            ++fails;
        }

        check(!m3.set_tileset("no_such_tileset.png"),
              "不存在的图集应该返回 false");
    }

    // ---------------------------------------------------------------
    // 7) 空/边界参数
    // ---------------------------------------------------------------
    {
        world::TileMap empty;
        empty.paint(0, 0, 5);              // w=0,h=0，应该静默忽略
        empty.set_solid(0, 0, true);
        check(empty.tile_count() == 0, "空 TileMap 的 tile_count 应为 0");
        check(empty.save("_edit_empty.map") == true, "空地图也能存（0×0 是合法的）");
        std::printf("[7] 空 TileMap 不崩\n");
    }

    std::printf("\n%s（失败 %d 项）\n", fails ? "未通过" : "全部通过", fails);
    return fails ? 1 : 0;
}
