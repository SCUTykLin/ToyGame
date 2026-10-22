#include "world/Props.h"
#include "world/AssetBank.h"
#include "world/TileMap.h"        // 为了 TileMap::TS

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace world {

namespace {

// 这张表就是"素材用在哪"的答案。
// 全部来自 backgrounds/ —— 那个目录里除了视差背景，
// 还有一批 western / medieval 包的单图道具，尺寸正好。
//
// 顺序 = 编号。第 i 行（从 0 数）的 id 就是 i+1，地图里存的就是这个数。
// 只能在**末尾追加**，不能插入、不能删除 —— 否则老地图全部错位。
const PropDef kProps[] = {
    // ---- 树与植被 ----
    { "backgrounds/tree-1.png",           0,0,0,0,  true,  true  },
    { "backgrounds/tree-2.png",           0,0,0,0,  true,  true  },
    { "backgrounds/bush.png",             0,0,0,0,  false, true  },
    { "backgrounds/rolling-bush.png",     0,0,0,0,  false, true  },
    { "backgrounds/branch.png",           0,0,0,0,  false, false },
    { "backgrounds/cactus-1.png",         0,0,0,0,  true,  true  },
    { "backgrounds/cactus-2.png",         0,0,0,0,  false, true  },
    { "backgrounds/cactus-3.png",         0,0,0,0,  true,  true  },

    // ---- 石头 ----
    { "backgrounds/rock-4.png",           0,0,0,0,  true,  true  },
    { "backgrounds/rock-5.png",           0,0,0,0,  false, true  },
    { "backgrounds/rock-6.png",           0,0,0,0,  false, false },

    // ---- 村庄杂物 ----
    { "backgrounds/barrel.png",           0,0,0,0,  true,  true  },
    { "backgrounds/explosive-barrel.png", 0,0,0,0,  true,  true  },
    { "backgrounds/crate-1.png",          0,0,0,0,  true,  true  },
    { "backgrounds/crate-2.png",          0,0,0,0,  true,  true  },
    { "backgrounds/jar-1.png",            0,0,0,0,  false, true  },
    { "backgrounds/jar-2.png",            0,0,0,0,  false, true  },
    { "backgrounds/water-trough.png",     0,0,0,0,  true,  true  },
    { "backgrounds/chariot.png",          0,0,0,0,  true,  true  },
    { "backgrounds/ladder.png",           0,0,0,0,  false, false },
    { "backgrounds/barrier.png",          0,0,0,0,  true,  true  },

    // ---- 可交互（Game 里按 E 有反应）----
    { "backgrounds/chest.png",            0,0,0,0,  true,  true,  true },
    { "backgrounds/sign-1.png",           0,0,0,0,  true,  true,  true },
    { "backgrounds/sign-2.png",           0,0,0,0,  true,  true,  true },

    // ---- 篝火：83×32 是 5 帧横排，取第 0 帧 ----
    { "backgrounds/fire.png",             0,0,17,32, false, true,  true },

    // ---- 地窖 ----
    { "backgrounds/grave-1.png",          0,0,0,0,  true,  true  },
    { "backgrounds/grave-2.png",          0,0,0,0,  true,  true  },
    { "backgrounds/grave-3.png",          0,0,0,0,  true,  true  },
    { "backgrounds/skull.png",            0,0,0,0,  false, false },
    { "backgrounds/bone.png",             0,0,0,0,  false, false },
    { "backgrounds/piano.png",            0,0,0,0,  true,  true  },

    // ---- 裂隙 ----
    { "backgrounds/meteor-1.png",         0,0,0,0,  true,  true  },
    { "backgrounds/planet-1.png",         0,0,0,0,  false, false },
};

constexpr int kPropCount = int(sizeof(kProps) / sizeof(kProps[0]));

// decor 是 uint8_t：0 = 空，1..255 = 道具
constexpr int kMaxProps = 255;

} // namespace

// ---------------------------------------------------------------
void PropSet::recalc(int i) {
    Item& it = items_[size_t(i)];

    // 源尺寸默认"整张图"。第一次算的时候才去解码。
    if (it.sw <= 0 || it.sh <= 0) {
        if (it.asset >= 0) {
            const gfx::Surface& img = bank_->image(it.asset);
            it.sw = std::max(1, img.width());
            it.sh = std::max(1, img.height());
        } else {
            it.sw = 32;                      // 图不在：占位 1 格
            it.sh = 32;
        }
        if (it.sx > it.sw) it.sx = 0;
        if (it.sy > it.sh) it.sy = 0;
    }

    it.dw = std::max(1, it.sw * it.scale / 100);
    it.dh = std::max(1, it.sh * it.scale / 100);

    // 底部中点对齐格子底部中点：
    // 一个 146×187 的树，ax = (32-146)/2 = -57，ay = 32-187 = -155，
    // 于是它从当前格子向上长 5 格。
    it.ax = (TileMap::TS - it.dw) / 2 + it.ox;
    it.ay = TileMap::TS - it.dh + it.oy;
}

void PropSet::append(const std::string& rel, int sx, int sy, int sw, int sh,
                     bool solid, bool tall, bool interactive, bool custom) {
    Item it;
    it.rel         = rel;
    it.sx = sx; it.sy = sy; it.sw = sw; it.sh = sh;
    it.solid       = solid;
    it.tall        = tall;
    it.interactive = interactive;
    it.custom      = custom;

    it.asset = bank_->index_of(rel);
    if (it.asset >= 0) bank_->pin(it.asset);   // 当道具用的图不许被 trim 掉

    items_.push_back(std::move(it));
    recalc(int(items_.size()) - 1);
}

void PropSet::load_manifest(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::printf("[props] 没有 %s，只用内置表\n", path.c_str());
        return;
    }

    std::string line;
    int added = 0, fixed = 0, dropped = 0;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        int  id   = 0;
        std::string rel;
        int  solid = 0, tall = 1, inter = 0;
        int  sx = 0, sy = 0, sw = 0, sh = 0, scale = 100, ox = 0, oy = 0;
        if (!(ss >> id >> rel)) continue;
        ss >> solid >> tall >> inter >> sx >> sy >> sw >> sh >> scale >> ox >> oy;

        if (id <= 0 || id > kMaxProps) { ++dropped; continue; }

        // id 必须连续。中间断了就补占位道具，否则后面全体错位。
        while (int(items_.size()) + 1 < id) {
            append("?", 0, 0, 0, 0, false, false, false, true);
            ++fixed;
            std::fprintf(stderr, "[props] id %d 前有空位，补一个占位道具\n",
                         int(items_.size()));
        }
        if (int(items_.size()) + 1 != id) { ++dropped; continue; }   // 重复 id

        append(rel, sx, sy, sw, sh, solid != 0, tall != 0, inter != 0, true);

        Item& it = items_.back();
        it.scale = std::clamp(scale, 10, 400);
        it.ox = ox;
        it.oy = oy;
        recalc(int(items_.size()) - 1);
        ++added;
    }

    std::printf("[props] %s：自定义 %d 个，补位 %d 个，丢弃 %d 个\n",
                path.c_str(), added, fixed, dropped);
}

bool PropSet::load(AssetBank& bank, const std::string& manifest) {
    bank_ = &bank;
    manifest_path_ = manifest;

    items_.clear();
    items_.reserve(size_t(kPropCount) + 32);

    for (int i = 0; i < kPropCount; ++i) {
        const PropDef& d = kProps[i];
        append(d.file, d.sx, d.sy, d.sw, d.sh, d.solid, d.tall, d.interactive, false);
    }

    load_manifest(manifest);

    std::printf("[props] 载入 %d 个装饰物（内置 %d + 自定义 %d）\n",
                count(), kPropCount, count() - kPropCount);
    return true;
}

void PropSet::reindex(AssetBank& bank) {
    bank_ = &bank;
    for (int i = 0; i < count(); ++i) {
        Item& it = items_[size_t(i)];
        it.asset = bank.index_of(it.rel);
        if (it.asset >= 0) bank.pin(it.asset);
    }
}

int PropSet::id_of(const std::string& rel) const {
    for (int i = 0; i < count(); ++i)
        if (items_[size_t(i)].rel == rel) return i + 1;
    return 0;
}

int PropSet::add(const std::string& rel, bool solid, bool tall, bool interactive) {
    const int old = id_of(rel);
    if (old) return old;
    if (count() >= kMaxProps) {
        // 这条故意用英文：中英混排的格式串在 MSVC 下会误报 C4477
        std::fprintf(stderr, "[props] table full (%d), skip %s\n", kMaxProps, rel.c_str());
        return 0;
    }
    append(rel, 0, 0, 0, 0, solid, tall, interactive, true);

    std::printf("[props] 注册道具 #%d %s\n", count(), rel.c_str());
    return count();
}

const gfx::Surface& PropSet::image(int i) {
    if (!valid(i)) return bank_->image(-1);
    return bank_->image(items_[size_t(i)].asset);
}

std::string PropSet::label(int i) const {
    if (!valid(i)) return "?";
    std::string s = items_[size_t(i)].rel;

    const size_t slash = s.find_last_of('/');
    if (slash != std::string::npos) s = s.substr(slash + 1);

    const size_t dot = s.find_last_of('.');
    if (dot != std::string::npos) s = s.substr(0, dot);
    return s;
}

void PropSet::set_flags(int i, bool s, bool t, bool in) {
    if (!valid(i)) return;
    Item& it = items_[size_t(i)];
    it.solid = s;
    it.tall = t;
    it.interactive = in;
}

void PropSet::set_scale(int i, int percent) {
    if (!valid(i)) return;
    items_[size_t(i)].scale = std::clamp(percent, 10, 400);
    recalc(i);
}

void PropSet::set_offset(int i, int x, int y) {
    if (!valid(i)) return;
    items_[size_t(i)].ox = std::clamp(x, -128, 128);
    items_[size_t(i)].oy = std::clamp(y, -128, 128);
    recalc(i);
}

bool PropSet::save_manifest(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        std::fprintf(stderr, "写道具表失败: %s\n", path.c_str());
        return false;
    }

    out << "# SIMDRPG props v1\n";
    out << "# id rel solid tall interactive sx sy sw sh scale ox oy\n";
    out << "# id 必须连续：从 34 开始，一个都不能跳。\n";

    int n = 0;
    for (int i = 0; i < count(); ++i) {
        const Item& it = items_[size_t(i)];
        if (!it.custom) continue;
        out << (i + 1) << ' ' << it.rel << ' '
            << int(it.solid) << ' ' << int(it.tall) << ' ' << int(it.interactive) << ' '
            << it.sx << ' ' << it.sy << ' ' << it.sw << ' ' << it.sh << ' '
            << it.scale << ' ' << it.ox << ' ' << it.oy << '\n';
        ++n;
    }

    std::printf("[props] 已保存 %s（%d 个自定义道具）\n", path.c_str(), n);
    return true;
}

} // namespace world
