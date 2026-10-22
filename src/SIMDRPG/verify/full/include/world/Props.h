#pragma once
#include "gfx/Surface.h"

#include <string>
#include <vector>

namespace world {

class AssetBank;
class TileMap;

// 一条装饰物的定义。只写"从哪张图的哪块"，尺寸和对齐在加载时算。
struct PropDef {
    const char* file;      // 相对 assets/gfx
    int  sx, sy, sw, sh;   // 源矩形；sw == 0 表示用整张图
    bool solid;            // 落笔时是否顺手把格子设成碰撞
    bool tall;             // true = 和实体一起按 y 排序（能挡住人）
                           // false = 贴地画（永远垫在最下面）
    bool interactive = false;   // 能不能按 E
};

// ---------------------------------------------------------------
// 可交互道具的编号（地图 [DECOR] 里存的值，1-based）
//
// ⚠ 这些必须和 Props.cpp 里 kProps[] 的顺序严格对应。
// 改表的时候顺手改这里，否则"按 E 开宝箱"会开成墓碑。
// ---------------------------------------------------------------
namespace prop_id {
    constexpr int Jar1  = 16;
    constexpr int Jar2  = 17;
    constexpr int Chest = 22;
    constexpr int Sign1 = 23;
    constexpr int Sign2 = 24;
    constexpr int Fire  = 25;
}

// ---------------------------------------------------------------
// 一张"道具表"：33 个内置 + 资产编辑器注册的自定义道具
//
// 关键设计：**道具不自己存像素**，只存 AssetBank 的下标。
// 这样 AssetBank 热重载一张树，地图上所有树下一帧就变了，
// 不需要通知任何人，也不需要重建任何东西。
//
// 地图里的 decor 是 uint8_t，所以道具编号只能是 1..255，
// 而且必须**连续**（id == 下标 + 1）—— 中间断一个，
// 后面所有道具的编号都会错位，整张地图的装饰物集体变成别的东西。
// ---------------------------------------------------------------
class PropSet {
public:
    // manifest 存自定义道具（编辑器保存的那部分）。文件不存在就只用内置表。
    bool load(AssetBank& bank, const std::string& manifest = "assets/props.cfg");

    // AssetBank 重扫之后下标会变，必须调一次把 rel 重新映射成 index。
    void reindex(AssetBank& bank);

    // 把某个素材注册成道具，返回 1-based id；表满（255）返回 0。
    // rel 已经在表里就直接返回原来的 id。
    int  add(const std::string& rel, bool solid, bool tall, bool interactive);

    // rel 对应的 id，0 = 没注册过
    int  id_of(const std::string& rel) const;

    int count() const { return int(items_.size()); }

    const gfx::Surface& image(int i);
    const std::string& rel(int i) const { return items_[size_t(i)].rel; }
    std::string  label(int i) const;          // 去掉目录和扩展名，给 UI 用

    int  sx(int i) const { return items_[size_t(i)].sx; }
    int  sy(int i) const { return items_[size_t(i)].sy; }
    int  sw(int i) const { return items_[size_t(i)].sw; }
    int  sh(int i) const { return items_[size_t(i)].sh; }
    int  dw(int i) const { return items_[size_t(i)].dw; }   // 落笔后的显示尺寸
    int  dh(int i) const { return items_[size_t(i)].dh; }
    int  ax(int i) const { return items_[size_t(i)].ax; }
    int  ay(int i) const { return items_[size_t(i)].ay; }

    bool solid(int i) const       { return items_[size_t(i)].solid; }
    bool tall(int i) const        { return items_[size_t(i)].tall; }
    bool interactive(int i) const { return items_[size_t(i)].interactive; }
    bool custom(int i) const      { return items_[size_t(i)].custom; }
    bool valid(int i) const       { return i >= 0 && i < count(); }

    // ---- 资产编辑器改属性 ----
    void set_flags(int i, bool solid, bool tall, bool interactive);
    void set_scale(int i, int percent);          // 10..400
    void set_offset(int i, int ox, int oy);      // 像素

    int scale(int i) const  { return items_[size_t(i)].scale; }
    int ox(int i) const     { return items_[size_t(i)].ox; }
    int oy(int i) const     { return items_[size_t(i)].oy; }

    // 只把 custom == true 的写出去（内置表在代码里，不需要存）
    bool save_manifest(const std::string& path) const;
    const std::string& manifest_path() const { return manifest_path_; }

private:
    struct Item {
        std::string rel;
        int  asset = -1;       // AssetBank 下标；-1 = 图不在
        int  sx = 0, sy = 0, sw = 0, sh = 0;
        int  dw = 0, dh = 0;   // 按 scale 算出来的显示尺寸
        int  ax = 0, ay = 0;   // 绘制偏移（底部中点对齐 + ox/oy）
        int  scale = 100;
        int  ox = 0, oy = 0;
        bool solid = false, tall = true, interactive = false;
        bool custom = false;
    };

    void recalc(int i);
    void load_manifest(const std::string& path);
    void append(const std::string& rel, int sx, int sy, int sw, int sh,
                bool solid, bool tall, bool interactive, bool custom);

    AssetBank* bank_ = nullptr;
    std::vector<Item> items_;
    std::string manifest_path_;
};

} // namespace world
