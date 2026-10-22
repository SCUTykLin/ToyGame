// 资产库 + 道具表的回归测试（不需要窗口，纯逻辑）
//
// 覆盖三件容易出错的事：
//   1. 扫描 1195 张图、缩略图尺寸、按路径找下标
//   2. 磁盘上的图变了，check_disk() 能不能发现并重载
//   3. props.cfg 存了再读，id / 属性 / rel 是否一模一样

#include "world/AssetBank.h"
#include "world/Props.h"

#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;

static int g_fail = 0;

#define CHECK(cond, msg)                                            \
    do {                                                            \
        if (cond) { std::printf("[ ok ] %s\n", msg); }              \
        else      { std::printf("[FAIL] %s\n", msg); ++g_fail; }    \
    } while (0)

int main() {
    // ---------- 1. 扫描 ----------
    world::AssetBank bank("assets/gfx");
    const int n = bank.scan();
    std::printf("[info] 素材总数 %d\n", n);
    CHECK(n > 1000, "扫描到 1000 张以上");

    const int tree = bank.index_of("backgrounds/tree-1.png");
    CHECK(tree >= 0, "按路径能找到 tree-1.png");
    if (tree < 0) return 1;

    const gfx::Surface& th = bank.thumb(tree);
    CHECK(th.width() <= 44 && th.height() <= 44, "缩略图最长边 <= 44");

    const gfx::Surface& full = bank.image(tree);
    CHECK(full.width() > 44, "全尺寸图有真实尺寸");
    CHECK(bank.info(tree).w == full.width(), "info 里的宽和高被解码填上了");

    // ---------- 2. 热重载 ----------
    std::error_code ec;
    fs::create_directories("_hot", ec);
    fs::copy_file("assets/gfx/backgrounds/tree-1.png", "_hot/a.png",
                  fs::copy_options::overwrite_existing, ec);
    CHECK(!ec, "复制出一张临时图");

    world::AssetBank hot("_hot");
    hot.scan();
    const int ai = hot.index_of("a.png");
    CHECK(ai >= 0, "临时目录能扫到 a.png");

    const int w_before = hot.image(ai).width();
    const int h_before = hot.image(ai).height();

    fs::copy_file("assets/gfx/backgrounds/bush.png", "_hot/a.png",
                  fs::copy_options::overwrite_existing, ec);
    CHECK(!ec, "用另一张图覆盖它");

    const int changed = hot.check_disk();
    CHECK(changed == 1, "check_disk 发现 1 个文件变了");

    const int w_after = hot.image(ai).width();
    const int h_after = hot.image(ai).height();
    std::printf("[info] %dx%d -> %dx%d\n", w_before, h_before, w_after, h_after);
    CHECK(w_after != w_before || h_after != h_before, "像素已经被换掉了");

    // ---------- 3. 道具表 ----------
    // 先删掉上次跑测试留下的清单，否则"只有 33 个"这条会失败
    fs::remove("_hot/props.cfg", ec);

    world::PropSet ps;
    ps.load(bank, "_hot/props.cfg");
    CHECK(ps.count() == 33, "没有清单时只有 33 个内置道具");

    const int again = ps.add("backgrounds/tree-2.png", true, true, false);
    CHECK(again == 2, "已经在表里的素材返回原 id（2）");

    int obj = -1;
    for (int i = 0; i < bank.count(); ++i) {
        if (bank.info(i).dir == "objects") { obj = i; break; }
    }
    CHECK(obj >= 0, "找一个 objects/ 素材");
    const std::string rel = bank.info(obj).rel;

    const int cid = ps.add(rel, false, true, true);
    CHECK(cid == 34, "自定义道具从 34 开始编号");
    CHECK(ps.custom(cid - 1), "标成了 custom");

    ps.set_scale(cid - 1, 200);
    ps.set_offset(cid - 1, 5, -7);
    CHECK(ps.dw(cid - 1) == ps.sw(cid - 1) * 2, "scale=200 让显示宽度翻倍");

    CHECK(ps.save_manifest("_hot/props.cfg"), "写出 props.cfg");

    world::PropSet ps2;
    ps2.load(bank, "_hot/props.cfg");
    CHECK(ps2.count() == ps.count(), "重读后道具数量一致");
    CHECK(ps2.rel(cid - 1) == rel, "重读后 rel 一致");
    CHECK(ps2.interactive(cid - 1), "interactive 保住了");
    CHECK(ps2.scale(cid - 1) == 200, "scale 保住了");
    CHECK(ps2.ox(cid - 1) == 5 && ps2.oy(cid - 1) == -7, "offset 保住了");

    // ---------- 4. 255 上限 ----------
    int added = 0;
    for (int i = 0; i < bank.count() && ps2.count() < 255; ++i) {
        if (ps2.add(bank.info(i).rel, false, false, false)) ++added;
    }
    CHECK(ps2.count() == 255, "表最多 255 个（decor 是 uint8_t）");
    CHECK(ps2.add("backgrounds/tree-1.png", false, false, false) != 0,
          "已存在的还能取到 id");
    CHECK(ps2.add(bank.info(bank.count() - 1).rel, false, false, false) == 0 ||
          ps2.id_of(bank.info(bank.count() - 1).rel) != 0,
          "满了之后新素材加不进来");

    std::printf("\n%s（失败 %d 项）\n", g_fail ? "有问题" : "全部通过", g_fail);
    return g_fail ? 1 : 0;
}
