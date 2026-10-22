// EditSession 的回归测试：**不需要窗口、不需要 ImGui**
//
// 这就是把编辑逻辑从 UI 里拆出来的好处：
// 画笔、撤销、落笔刷碰撞、保存，全都能在命令行里验证。

#include "world/AssetBank.h"
#include "world/EditSession.h"
#include "world/Props.h"
#include "world/TileMap.h"

#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;
using world::EditSession;

static int g_fail = 0;

#define CHECK(cond, msg)                                            \
    do {                                                            \
        if (cond) { std::printf("[ ok ] %s\n", msg); }              \
        else      { std::printf("[FAIL] %s\n", msg); ++g_fail; }    \
    } while (0)

int main() {
    std::error_code ec;
    fs::create_directories("_hot", ec);

    world::AssetBank bank("assets/gfx");
    bank.scan();

    world::TileMap map = world::TileMap::load("assets/maps/village.map");
    world::PropSet props;
    props.load(bank, "_hot/session_props.cfg");

    std::string map_path = "_hot/session.map";
    EditSession s;
    s.bind(&map, &props, &bank, &map_path);

    // ---------- 1. 地形：画一笔，撤销 ----------
    const int gx = 3, gy = 3;
    const int ground_before = map.at(gx, gy);

    s.set_layer(EditSession::Layer::Ground);
    s.set_brush(26);
    s.begin_stroke(gx, gy, false);
    s.end_stroke();

    CHECK(map.at(gx, gy) == 26, "左键把地面画成 26");
    CHECK(s.dirty(), "改过之后 dirty");

    s.undo();
    CHECK(map.at(gx, gy) == ground_before, "撤销把地面还原");

    // ---------- 2. 一笔覆盖多格，撤销要整笔回退 ----------
    const int a = map.at(5, 5), b = map.at(6, 6);
    s.begin_stroke(5, 5, false);
    s.paint_cell(6, 6, false);
    s.end_stroke();
    CHECK(map.at(5, 5) == 26 && map.at(6, 6) == 26, "一笔画了两格");
    s.undo();
    CHECK(map.at(5, 5) == a && map.at(6, 6) == b, "一次撤销回退整笔");

    // ---------- 3. 装饰：注册 + 落笔顺便刷碰撞 ----------
    s.select_asset(bank.index_of("backgrounds/chest.png"));
    const int id = s.ensure_selected_prop();
    CHECK(id == 22, "内置的 chest 复用 id 22");

    s.set_layer(EditSession::Layer::Decor);
    const bool solid_before = map.is_solid(10, 10);   // 这格可能本来就是墙
    s.begin_stroke(10, 10, false);
    s.end_stroke();

    CHECK(map.decor_at(10, 10) == 22, "装饰层写入 22");
    CHECK(map.is_solid(10, 10), "chest 的 solid=1 落笔时刷了碰撞");

    s.undo();
    CHECK(map.decor_at(10, 10) == 0, "撤销清掉装饰");
    // 撤销只回到"落笔前"，不是无脑清 0 —— 原来就是墙的格子还得是墙
    CHECK(map.is_solid(10, 10) == solid_before, "撤销把碰撞还原成落笔前的值");

    // ---------- 4. 中键吸管 ----------
    s.set_layer(EditSession::Layer::Ground);
    s.set_brush(5);
    s.pick(3, 3);
    CHECK(s.brush() == map.at(3, 3), "吸管把格子里的 tile 捡成画笔");

    // ---------- 5. 存盘 → 重读 ----------
    s.set_layer(EditSession::Layer::Decor);
    s.begin_stroke(12, 12, false);
    s.end_stroke();
    CHECK(s.save_all(), "保存地图 + props.cfg");
    CHECK(!s.dirty(), "保存后 dirty 清掉");

    world::TileMap again = world::TileMap::load(map_path);
    CHECK(again.decor_at(12, 12) == 22, "重读后装饰还在");
    CHECK(again.at(gx, gy) == map.at(gx, gy), "重读后地形一致");

    std::printf("\n%s（失败 %d 项）\n", g_fail ? "有问题" : "全部通过", g_fail);
    return g_fail ? 1 : 0;
}
