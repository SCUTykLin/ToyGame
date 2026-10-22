#include "ui/EditorUi.h"

#include "platform/TextureCache.h"
#include "world/AssetBank.h"
#include "world/Camera.h"
#include "world/EditSession.h"
#include "world/Props.h"
#include "world/TileMap.h"

#include "imgui.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace ui {

namespace {

constexpr int TS = world::TileMap::TS;

const char* kLayerName[3] = { "GROUND", "SOLID", "DECOR" };

// 不区分大小写的子串查找（文件名都是 ASCII，够用）
bool contains_ci(const std::string& hay, const char* needle) {
    if (!needle || !needle[0]) return true;
    std::string h = hay, n = needle;
    for (char& c : h) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
    for (char& c : n) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
    return h.find(n) != std::string::npos;
}

} // namespace

// ---------------------------------------------------------------
void EditorUi::init(world::EditSession* session,
                    world::TileMap* map, world::PropSet* props, world::AssetBank* bank,
                    world::Camera* cam, platform::TextureCache* tex,
                    int fb_w, int fb_h, std::string* map_path) {
    session_ = session;
    map_ = map;
    props_ = props;
    bank_ = bank;
    cam_ = cam;
    tex_ = tex;
    fb_w_ = fb_w;
    fb_h_ = fb_h;
    map_path_ = map_path;

    brush_size_ = session_ ? session_->brush_size() : 1;

    // 打开编辑器时先选中一棵树：右边有东西看，DECOR 层直接能落笔
    if (session_) {
        session_->select_asset(bank_->index_of("backgrounds/tree-1.png"));
        session_->select_prop(0);
    }
}

void EditorUi::toggle() { set_active(!active_); }

void EditorUi::set_active(bool on) {
    active_ = on;
    if (on) {
        filter_dirty_ = true;
        set_status("F2 close / F5 save / Ctrl+Z undo / 1-3 layer / [ ] brush size");
    } else {
        placing_ = false;
    }
}

void EditorUi::set_status(const std::string& s) {
    status_ = s;
    status_t_ = 3.5f;
    std::printf("[editor] %s\n", s.c_str());
}

void EditorUi::switch_layer(int layer) {
    if (session_) session_->set_layer(world::EditSession::Layer(layer));
    layer_ = layer;
    set_status(std::string("layer = ") + kLayerName[layer]);
}

void EditorUi::save() {
    if (session_ && session_->save_all()) {
        set_status("saved map + props.cfg");
    } else {
        set_status("save failed（看控制台）");
    }
}

// ---------------------------------------------------------------
// 画布：一整屏的隐形按钮 + 自绘网格
//
// 为什么不用 ImGui::Image 把游戏画面再贴一遍：
//   游戏画面已经在窗口上了。这里只需要一个"能接收鼠标的矩形"，
//   用 InvisibleButton 最省事，还自动处理了层级（面板盖住时不算悬停）。
// ---------------------------------------------------------------
void EditorUi::draw_canvas() {
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##canvas", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::InvisibleButton("##paint", ImGui::GetContentRegionAvail(),
                           ImGuiButtonFlags_MouseButtonLeft |
                           ImGuiButtonFlags_MouseButtonRight |
                           ImGuiButtonFlags_MouseButtonMiddle);
    const bool hovered = ImGui::IsItemHovered();

    ImGuiIO& io = ImGui::GetIO();
    const float s = (fb_w_ > 0) ? (io.DisplaySize.x / float(fb_w_)) : 1.f;

    // 鼠标（窗口像素）→ 世界像素 → 格子。
    // 这里多了一个 s：画面是 640×360，窗口是 1280×720，鼠标坐标要除以 2。
    const int wx = cam_->x + int(io.MousePos.x / s);
    const int wy = cam_->y + int(io.MousePos.y / s);
    const int tx = wx / TS;
    const int ty = wy / TS;
    const bool in_map = (tx >= 0 && ty >= 0 && tx < map_->w && ty < map_->h);

    // ---- 画图：网格 + 碰撞 + 实体标记 ----
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImU32 grid_col = IM_COL32(255, 255, 255, 28);
    for (int x = -(cam_->x % TS); x < fb_w_; x += TS)
        dl->AddLine(ImVec2(x * s, 0), ImVec2(x * s, io.DisplaySize.y), grid_col);
    for (int y = -(cam_->y % TS); y < fb_h_; y += TS)
        dl->AddLine(ImVec2(0, y * s), ImVec2(io.DisplaySize.x, y * s), grid_col);

    if (layer_ == 1) {
        for (int y = std::max(0, cam_->y / TS); y <= std::min(map_->h - 1, (cam_->y + fb_h_) / TS); ++y)
            for (int x = std::max(0, cam_->x / TS); x <= std::min(map_->w - 1, (cam_->x + fb_w_) / TS); ++x)
                if (map_->is_solid(x, y)) {
                    const ImVec2 a((x * TS - cam_->x) * s, (y * TS - cam_->y) * s);
                    dl->AddRectFilled(a, ImVec2(a.x + TS * s, a.y + TS * s),
                                      IM_COL32(255, 40, 40, 60));
                }
    }

    for (const auto& sp : map_->spawns) {
        const ImVec2 a((sp.x * TS - cam_->x) * s, (sp.y * TS - cam_->y) * s);
        ImU32 c = IM_COL32(255, 64, 64, 200);
        if (sp.type.rfind("npc_", 0) == 0) c = IM_COL32(64, 255, 64, 200);
        else if (sp.type == "portal")      c = IM_COL32(64, 144, 255, 200);
        dl->AddRectFilled(a, ImVec2(a.x + 16 * s, a.y + 16 * s), c);
    }

    const int bsize = session_ ? session_->brush_size() : 1;
    if (in_map) {
        const int r = bsize / 2;
        const ImVec2 a(((tx - r) * TS - cam_->x) * s, ((ty - r) * TS - cam_->y) * s);
        const ImVec2 b(((tx - r + bsize) * TS - cam_->x) * s,
                       ((ty - r + bsize) * TS - cam_->y) * s);
        dl->AddRect(a, b, IM_COL32(255, 220, 80, 255), 0.f, 0, 2.f);
    }

    // ---- 鼠标：按下 → 开一笔；拖动 → 继续；松开 → 收笔 ----
    if (hovered && in_map && session_) {
        if (io.MouseClicked[0]) {
            placing_ = true;
            session_->begin_stroke(tx, ty, false);
        }
        if (io.MouseClicked[1]) {
            placing_ = true;
            session_->begin_stroke(tx, ty, true);
        }
        if (io.MouseClicked[2]) {
            session_->pick(tx, ty);
            set_status(layer_ == 0 ? ("pick tile " + std::to_string(session_->brush()))
                                   : ("pick prop #" + std::to_string(session_->selected_prop() + 1)));
        }
    }

    if (placing_ && in_map && session_) {
        // 用按键状态判断左右键，而不是 Clicked —— 拖动时要连续触发
        if (io.MouseDown[0])      session_->paint_cell(tx, ty, false);
        else if (io.MouseDown[1]) session_->paint_cell(tx, ty, true);
    }

    // 注意：松开判断不能只看 hovered（鼠标可能拖到面板上再松开）
    if (placing_ && !io.MouseDown[0] && !io.MouseDown[1]) {
        if (session_) session_->end_stroke();
        placing_ = false;
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

// ---------------------------------------------------------------
void EditorUi::draw_map_panel() {
    ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(272, 636), ImGuiCond_Always);
    ImGui::Begin("MAP", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    ImGui::Text("%s  %dx%d", map_->name.c_str(), map_->w, map_->h);
    ImGui::TextDisabled("%s  tpr=%d", map_->tileset.c_str(), map_->tpr);

    ImGui::Separator();
    for (int i = 0; i < 3; ++i) {
        if (i) ImGui::SameLine();
        if (ImGui::RadioButton(kLayerName[i], layer_ == i)) switch_layer(i);
    }

    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##brush", &brush_size_, 1, 5, "brush %d");
    if (brush_size_ % 2 == 0) ++brush_size_;     // 只允许 1/3/5
    if (session_ && brush_size_ != session_->brush_size()) session_->set_brush_size(brush_size_);

    ImGui::Separator();

    if (layer_ == 0) {
        const int brush = session_ ? session_->brush() : 17;
        ImGui::TextDisabled("tile %d / %d", brush, std::max(0, map_->tile_count() - 1));

        // 图集：ImGui 要纹理句柄，这里把 TileMap::image 传到 GPU。
        // stamp 用尺寸拼一下就行 —— 换图集时尺寸/tileset 必然变。
        const uint64_t stamp = (uint64_t(map_->image.width()) << 32) |
                                uint32_t(map_->image.height());
        void* atlas = tex_->get(map_->image, "atlas:" + map_->tileset, stamp);

        if (atlas) {
            if (ImGui::BeginChild("##palette", ImVec2(0, 0), true)) {
                const float cell = 30.f;
                const int   per_row = 8;
                const int   tw = map_->image.width();
                const int   th = map_->image.height();
                for (int i = 0; i < map_->tile_count(); ++i) {
                    const int cx = i % map_->tpr;
                    const int cy = i / map_->tpr;
                    const ImVec2 uv0(float(cx * TS) / float(tw), float(cy * TS) / float(th));
                    const ImVec2 uv1(float(cx * TS + TS) / float(tw), float(cy * TS + TS) / float(th));

                    ImGui::PushID(i);
                    if (ImGui::ImageButton("##t", (ImTextureID)atlas, ImVec2(cell, cell), uv0, uv1)) {
                        if (session_) session_->set_brush(i);
                    }
                    ImGui::PopID();
                    if ((i + 1) % per_row) ImGui::SameLine();
                }
            }
            ImGui::EndChild();
        }
    } else if (layer_ == 1) {
        ImGui::TextWrapped("LMB paint wall, RMB erase. Red = solid.");
        ImGui::TextDisabled("Outside the map is always solid.");
    } else {
        const int sp = session_ ? session_->selected_prop() : 0;
        if (!props_->valid(sp)) {
            ImGui::TextWrapped("Pick an image in the ASSETS panel first.");
        } else {
            ImGui::Text("prop #%d  %s", sp + 1, props_->label(sp).c_str());
            ImGui::TextDisabled("src %dx%d  draw %dx%d",
                                props_->sw(sp), props_->sh(sp),
                                props_->dw(sp), props_->dh(sp));

            bool so = props_->solid(sp);
            bool ta = props_->tall(sp);
            bool in = props_->interactive(sp);
            if (ImGui::Checkbox("solid (paint collision)", &so))
                session_->set_prop_flags(sp, so, ta, in);
            if (ImGui::Checkbox("tall (y-sorted)", &ta))
                session_->set_prop_flags(sp, so, ta, in);
            if (ImGui::Checkbox("interactive (press E)", &in))
                session_->set_prop_flags(sp, so, ta, in);

            int scale = props_->scale(sp);
            if (ImGui::SliderInt("scale %", &scale, 10, 400))
                session_->set_prop_scale(sp, scale);

            int ox = props_->ox(sp);
            int oy = props_->oy(sp);
            if (ImGui::DragInt("offset x", &ox, 1, -64, 64)) session_->set_prop_offset(sp, ox, oy);
            if (ImGui::DragInt("offset y", &oy, 1, -64, 64)) session_->set_prop_offset(sp, ox, oy);

            ImGui::TextDisabled("LMB place / RMB erase / MMB pick");
        }
    }

    ImGui::End();
}

// ---------------------------------------------------------------
void EditorUi::rebuild_filter() {
    view_.clear();
    cats_.clear();

    // 分类 = 顶层目录，外加一个"* all"
    cats_.push_back("*");
    for (int i = 0; i < bank_->count(); ++i) {
        const std::string& d = bank_->info(i).dir;
        if (std::find(cats_.begin() + 1, cats_.end(), d) == cats_.end())
            cats_.push_back(d);
    }
    std::sort(cats_.begin() + 1, cats_.end());
    asset_cat_ = std::clamp(asset_cat_, 0, int(cats_.size()) - 1);

    const std::string& cat = cats_[size_t(asset_cat_)];
    for (int i = 0; i < bank_->count(); ++i) {
        const world::AssetInfo& a = bank_->info(i);
        if (cat != "*" && a.dir != cat) continue;
        if (!contains_ci(a.rel, search_)) continue;
        view_.push_back(i);
    }
    page_ = 0;
    filter_dirty_ = false;
}

void EditorUi::draw_asset_panel() {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 360.f, 8), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(352, 636), ImGuiCond_Always);
    ImGui::Begin("ASSETS", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    if (filter_dirty_ || cats_.empty()) rebuild_filter();

    if (ImGui::InputTextWithHint("##search", "search filename...", search_, sizeof(search_)))
        filter_dirty_ = true;

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##cat", cats_[size_t(asset_cat_)].c_str())) {
        for (int i = 0; i < int(cats_.size()); ++i) {
            if (ImGui::Selectable(cats_[size_t(i)].c_str(), i == asset_cat_)) {
                asset_cat_ = i;
                filter_dirty_ = true;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::TextDisabled("%d files   [%d/%d]", int(view_.size()), page_ + 1,
                        std::max(1, int((view_.size() + cells_per_page() - 1) / cells_per_page())));

    // ---- 缩略图网格 ----
    const int per = cells_per_page();
    const int first = page_ * per;

    if (ImGui::BeginChild("##grid", ImVec2(0, 300), true)) {
        const float cell = 62.f;
        for (int i = first; i < std::min(int(view_.size()), first + per); ++i) {
            const int ai = view_[size_t(i)];
            const gfx::Surface& img = bank_->thumb(ai);
            // stamp 用素材库的版本号：文件一改，ImGui 这边的纹理也重建
            void* t = tex_->get(img, bank_->info(ai).rel, bank_->stamp(ai));

            ImGui::PushID(ai);
            if (t) {
                if (ImGui::ImageButton("##a", (ImTextureID)t, ImVec2(cell, cell))) {
                    if (session_) session_->select_asset(ai);
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s\n%d x %d", bank_->info(ai).rel.c_str(),
                                      bank_->info(ai).w, bank_->info(ai).h);
            }
            ImGui::PopID();

            if ((i - first + 1) % 5) ImGui::SameLine();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("<")) { page_ = std::max(0, page_ - 1); filter_dirty_ = true; }
    ImGui::SameLine();
    if (ImGui::Button(">")) {
        const int last = std::max(0, int((view_.size() - 1) / per));
        page_ = std::min(last, page_ + 1);
        filter_dirty_ = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("rescan disk")) {
        const int n = session_ ? session_->rescan_assets() : 0;
        filter_dirty_ = true;
        set_status("rescan: " + std::to_string(n) + " files");
    }

    // ---- 选中预览 ----
    ImGui::Separator();
    const int sel = session_ ? session_->selected_asset() : -1;
    if (bank_->valid(sel)) {
        const world::AssetInfo& a = bank_->info(sel);
        ImGui::Text("%s", a.name.c_str());
        ImGui::TextDisabled("%s  %dx%d", a.dir.c_str(), a.w, a.h);

        const gfx::Surface& full = bank_->image(sel);
        void* t = tex_->get(full, a.rel, bank_->stamp(sel));
        if (t) {
            // 等比缩放到 96×96 的框里
            const float k = std::min(96.f / std::max(1, full.width()),
                                     96.f / std::max(1, full.height()));
            ImGui::Image((ImTextureID)t, ImVec2(full.width() * k, full.height() * k));
        }

        if (ImGui::Button("register as prop") && session_) session_->ensure_selected_prop();
        ImGui::SameLine();
        if (ImGui::Button("paint it")) {
            if (session_) {
                session_->ensure_selected_prop();
                session_->set_layer(world::EditSession::Layer::Decor);
                layer_ = 2;
            }
        }
        ImGui::TextDisabled("id = %d / 255", props_->id_of(a.rel));
    } else {
        ImGui::TextDisabled("(no asset selected)");
    }

    ImGui::End();
}

// ---------------------------------------------------------------
void EditorUi::draw_status_bar() {
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(0, vp->Size.y - 36.f));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, 36.f));
    ImGui::Begin("##status", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (ImGui::Button("Save (F5)")) save();
    ImGui::SameLine();
    if (ImGui::Button("Reload map")) reload_clicked_ = true;
    ImGui::SameLine();
    if (ImGui::Button("Undo") && session_) session_->undo();
    ImGui::SameLine();

    const bool dirty = session_ && session_->dirty();
    ImGui::Text("  %s   %s", map_path_ ? map_path_->c_str() : "?",
                dirty ? "*unsaved*" : "clean");

    // 右对齐：先算出这行字多宽，再把光标挪过去。
    // 别用固定的 x - 470 —— 窗口一改宽度就叠字。
    char right[160];
    std::snprintf(right, sizeof(right), "hot reload: %d   tex: %d   mem: %.1f MB   ",
                  bank_->reload_count(), tex_->count(),
                  double(bank_->memory_bytes()) / (1024.0 * 1024.0));
    const float tw = ImGui::CalcTextSize(right).x;
    ImGui::SameLine(ImGui::GetWindowWidth() - tw - 8.f);
    ImGui::TextDisabled("%s", right);

    ImGui::End();
}

// ---------------------------------------------------------------
void EditorUi::draw(bool* reload_requested) {
    if (!active_ || !session_) return;

    ImGuiIO& io = ImGui::GetIO();
    if (status_t_ > 0.f) status_t_ -= io.DeltaTime;

    // 相机自己挪（编辑时相机不跟玩家）
    if (!io.WantTextInput) {
        const float v = 520.f * io.DeltaTime;
        const int   dx = (ImGui::IsKeyDown(ImGuiKey_A) ? -1 : 0) + (ImGui::IsKeyDown(ImGuiKey_D) ? 1 : 0);
        const int   dy = (ImGui::IsKeyDown(ImGuiKey_W) ? -1 : 0) + (ImGui::IsKeyDown(ImGuiKey_S) ? 1 : 0);
        cam_->x += int(dx * v);
        cam_->y += int(dy * v);

        const int max_x = std::max(0, map_->pixel_width() - fb_w_);
        const int max_y = std::max(0, map_->pixel_height() - fb_h_);
        cam_->x = std::clamp(cam_->x, 0, max_x);
        cam_->y = std::clamp(cam_->y, 0, max_y);
        cam_->shake_time = 0.f;
    }

    draw_canvas();
    draw_map_panel();
    draw_asset_panel();
    draw_status_bar();

    // ---- 快捷键 ----
    if (!io.WantTextInput) {
        if (ImGui::IsKeyPressed(ImGuiKey_F5)) save();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) session_->undo();
        if (ImGui::IsKeyPressed(ImGuiKey_1)) switch_layer(0);
        if (ImGui::IsKeyPressed(ImGuiKey_2)) switch_layer(1);
        if (ImGui::IsKeyPressed(ImGuiKey_3)) switch_layer(2);
        if (ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) {
            brush_size_ = std::max(1, brush_size_ - 2);
            session_->set_brush_size(brush_size_);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_RightBracket)) {
            brush_size_ = std::min(5, brush_size_ + 2);
            session_->set_brush_size(brush_size_);
        }
    }

    // 提示条：独立的小窗口浮在底部中间。
    // 塞进状态栏会和右边的统计叠字；而且提示条不该挤掉常驻信息。
    if (status_t_ > 0.f && !status_.empty()) {
        // 居中：先量一下这行字多宽，再算左上角。
        // 不要用 SetNextWindowPos 的 pivot 版本 + AlwaysAutoResize ——
        // 尺寸要下一帧才知道，位置会算歪（踩过一次）。
        const float tw2 = ImGui::CalcTextSize(status_.c_str()).x + 18.f;
        ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - tw2) * 0.5f, io.DisplaySize.y - 96.f),
                                ImGuiCond_Always);
        if (ImGui::Begin("##toast", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing)) {
            ImGui::TextColored(ImVec4(1.f, 0.85f, 0.4f, 1.f), "%s", status_.c_str());
        }
        ImGui::End();
    }

    if (reload_requested) *reload_requested = reload_clicked_;
    reload_clicked_ = false;
}

} // namespace ui
