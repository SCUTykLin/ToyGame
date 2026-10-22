#include "world/EditSession.h"

#include "world/AssetBank.h"
#include "world/Props.h"
#include "world/TileMap.h"

#include <algorithm>
#include <cstdio>

namespace world {

namespace {
constexpr int kMaxUndo = 64;
constexpr int kGroundIndex = 17;   // LPC 图集里的纯草地
}

void EditSession::bind(TileMap* map, PropSet* props, AssetBank* bank, std::string* map_path) {
    map_ = map;
    props_ = props;
    bank_ = bank;
    map_path_ = map_path;
}

void EditSession::set_brush_size(int n) {
    // 只允许 1 / 3 / 5：偶数画笔没有"中心格"，落笔会歪半格
    brush_size_ = (n <= 1) ? 1 : (n <= 3 ? 3 : 5);
}

void EditSession::select_prop(int i) {
    if (props_ && props_->valid(i)) sel_prop_ = i;
}

bool EditSession::valid_cell(int x, int y) const {
    return map_ && x >= 0 && y >= 0 && x < map_->w && y < map_->h;
}

// ---------------------------------------------------------------
void EditSession::begin_stroke(int tx, int ty, bool erase) {
    stroking_ = true;
    current_.cells.clear();
    paint_cell(tx, ty, erase);
}

void EditSession::paint_cell(int tx, int ty, bool erase) {
    if (!valid_cell(tx, ty)) return;

    const int r = brush_size_ / 2;
    for (int oy = -r; oy <= r; ++oy) {
        for (int ox = -r; ox <= r; ++ox) {
            const int x = tx + ox;
            const int y = ty + oy;
            if (!valid_cell(x, y)) continue;

            // 先记旧值。同一格在一笔里只记一次，否则撤销要退好几步。
            bool seen = false;
            for (const auto& c : current_.cells) {
                if (c.x == x && c.y == y) { seen = true; break; }
            }
            if (!seen) {
                current_.cells.push_back({ x, y, map_->at(x, y),
                                           map_->is_solid(x, y) ? 1 : 0,
                                           map_->decor_at(x, y) });
            }

            switch (layer_) {
                case Layer::Ground:
                    map_->paint(x, y, erase ? kGroundIndex : brush_);
                    break;

                case Layer::Solid:
                    map_->set_solid(x, y, !erase);
                    break;

                case Layer::Decor:
                    if (erase) {
                        map_->paint_decor(x, y, 0);
                    } else if (props_ && props_->valid(sel_prop_)) {
                        map_->paint_decor(x, y, sel_prop_ + 1);
                        // 道具的 solid 只是"落笔时的默认值"：
                        // 碰撞网格只有一张，真正挡人的是它。
                        if (props_->solid(sel_prop_)) map_->set_solid(x, y, true);
                    }
                    break;
            }
        }
    }

    dirty_ = true;
}

void EditSession::end_stroke() {
    stroking_ = false;
    if (current_.cells.empty()) return;

    undo_.push_back(current_);
    if (int(undo_.size()) > kMaxUndo) undo_.erase(undo_.begin());
    current_.cells.clear();
}

void EditSession::undo() {
    if (undo_.empty()) return;

    const Stroke& st = undo_.back();
    for (const auto& c : st.cells) {
        map_->paint(c.x, c.y, c.ground);
        map_->set_solid(c.x, c.y, c.solid != 0);
        map_->paint_decor(c.x, c.y, c.decor);
    }
    undo_.pop_back();
    dirty_ = true;
}

void EditSession::pick(int tx, int ty) {
    if (!valid_cell(tx, ty)) return;

    if (layer_ == Layer::Ground) {
        brush_ = map_->at(tx, ty);
    } else if (layer_ == Layer::Decor) {
        const int d = map_->decor_at(tx, ty);
        if (d > 0 && props_ && props_->valid(d - 1)) sel_prop_ = d - 1;
    }
}

// ---------------------------------------------------------------
int EditSession::add_asset_as_prop(int asset) {
    if (!bank_ || !props_) return 0;
    if (!bank_->valid(asset)) return 0;

    const std::string& rel = bank_->info(asset).rel;
    int id = props_->id_of(rel);
    if (!id) id = props_->add(rel, false, true, false);   // 默认 solid=false, tall=true

    if (id) {
        sel_prop_ = id - 1;
        dirty_ = true;
    }
    return id;
}

int EditSession::ensure_selected_prop() {
    return add_asset_as_prop(sel_asset_);
}

// ---------------------------------------------------------------
bool EditSession::set_prop_flags(int i, bool solid, bool tall, bool interactive) {
    if (!props_ || !props_->valid(i)) return false;
    props_->set_flags(i, solid, tall, interactive);
    dirty_ = true;
    return true;
}

void EditSession::set_prop_scale(int i, int percent) {
    if (!props_ || !props_->valid(i)) return;
    props_->set_scale(i, percent);
    dirty_ = true;
}

void EditSession::set_prop_offset(int i, int ox, int oy) {
    if (!props_ || !props_->valid(i)) return;
    props_->set_offset(i, ox, oy);
    dirty_ = true;
}

int EditSession::rescan_assets() {
    if (!bank_) return 0;
    const int n = bank_->scan();
    if (props_) props_->reindex(*bank_);
    return n;
}

bool EditSession::save_all() {
    bool ok = false;

    if (map_ && map_path_ && !map_path_->empty()) {
        ok = map_->save(*map_path_);
    }
    if (props_) {
        ok = props_->save_manifest(props_->manifest_path()) && ok;
    }

    if (ok) dirty_ = false;
    return ok;
}

} // namespace world
