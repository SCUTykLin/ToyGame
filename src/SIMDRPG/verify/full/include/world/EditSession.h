#pragma once

#include <string>
#include <vector>

namespace world {

class TileMap;
class PropSet;
class AssetBank;

// ---------------------------------------------------------------
// 编辑器的**纯逻辑**
//
// 这个类不认识 SDL、不认识 ImGui、不认识鼠标。
// 它只接受"第几格、画还是擦"，然后改 TileMap / PropSet。
//
// 为什么单独拆出来（对应 01 章的分层）：
//   1. 能无头测试 —— tests/edit_test.cpp 直接调它，不用开窗口；
//   2. UI 换掉（手搓 → ImGui → 以后换别的）它一行不用改；
//   3. 撤销、编号、保存这些"规则"集中在一处，不会在面板代码里散开。
//
// 依赖方向：world 层内部（TileMap / Props / AssetBank），不向上依赖。
// ---------------------------------------------------------------
class EditSession {
public:
    enum class Layer { Ground = 0, Solid = 1, Decor = 2 };

    void bind(TileMap* map, PropSet* props, AssetBank* bank, std::string* map_path);

    // ---- 当前编辑状态 ----
    Layer layer() const { return layer_; }
    void  set_layer(Layer l) { layer_ = l; }

    int  brush() const { return brush_; }
    void set_brush(int index) { brush_ = index; }

    int  brush_size() const { return brush_size_; }
    void set_brush_size(int n);          // 只接受 1/3/5

    int  selected_asset() const { return sel_asset_; }
    void select_asset(int bank_index) { sel_asset_ = bank_index; }

    int  selected_prop() const { return sel_prop_; }   // 0-based
    void select_prop(int i);

    // ---- 一次拖动 = 一笔 ----
    void begin_stroke(int tx, int ty, bool erase);
    void paint_cell(int tx, int ty, bool erase);
    void end_stroke();
    bool stroking() const { return stroking_; }

    // 中键吸管：Ground 捡 tile，Decor 捡道具
    void pick(int tx, int ty);

    // ---- 撤销 ----
    void undo();
    int  undo_depth() const { return int(undo_.size()); }

    // ---- 道具注册 ----
    // 把素材库里第 asset 个素材注册成道具，返回 1-based id（0 = 失败）
    int  add_asset_as_prop(int asset);
    int  ensure_selected_prop();          // 成功时顺便选中它

    // ---- 道具属性（UI 只发意图，改还是在这里改）----
    bool set_prop_flags(int i, bool solid, bool tall, bool interactive);
    void set_prop_scale(int i, int percent);
    void set_prop_offset(int i, int ox, int oy);

    // 重扫素材目录：AssetBank 的下标会变，道具表要重新映射
    int  rescan_assets();

    // ---- 保存：地图 + 道具表 ----
    bool save_all();

    bool dirty() const { return dirty_; }
    void clear_dirty() { dirty_ = false; }

private:
    struct CellEdit { int x, y, ground, solid, decor; };
    struct Stroke   { std::vector<CellEdit> cells; };

    bool valid_cell(int x, int y) const;

    TileMap*    map_   = nullptr;
    PropSet*    props_ = nullptr;
    AssetBank*  bank_  = nullptr;
    std::string* map_path_ = nullptr;

    Layer layer_ = Layer::Ground;
    int   brush_ = 17;
    int   brush_size_ = 1;
    int   sel_asset_ = -1;
    int   sel_prop_  = 0;
    bool  dirty_ = false;
    bool  stroking_ = false;

    Stroke current_;
    std::vector<Stroke> undo_;     // 最多 64 笔
};

} // namespace world
