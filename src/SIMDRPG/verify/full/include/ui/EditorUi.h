#pragma once

#include <string>
#include <vector>

namespace world {
class TileMap;
class PropSet;
class AssetBank;
class EditSession;
struct Camera;
}

namespace platform { class TextureCache; }

namespace ui {

// ---------------------------------------------------------------
// 编辑器的**界面层**（ImGui）
//
// 它只做三件事：
//   1. 把 EditSession 的状态画成面板；
//   2. 把鼠标位置换算成格子，转成"画/擦/吸"的意图；
//   3. 把按钮点成了哪件事告诉 EditSession。
//
// 它**不**直接改地图、不存撤销、不决定道具编号 —— 那些都在
// world/EditSession 里。所以：
//   - 换掉 ImGui（比如换成 Qt）只需要重写这个文件；
//   - EditSession 能被 tests/ 无头测试；
//   - 面板代码里不会出现 map_->paint(...) 这种"逻辑漏进 UI"的写法。
//
// 依赖方向：ui → world（EditSession/地图/道具/相机）、platform（纹理）、imgui。
// 它不认识 game/，所以 game 可以随便换，UI 不受影响。
// ---------------------------------------------------------------
class EditorUi {
public:
    void init(world::EditSession* session,
              world::TileMap* map, world::PropSet* props, world::AssetBank* bank,
              world::Camera* cam, platform::TextureCache* tex,
              int fb_w, int fb_h, std::string* map_path);

    void toggle();
    void set_active(bool on);
    bool active() const { return active_; }

    // 每帧一次（只在 active 时调）。
    // *reload_requested 置 true = 用户点了"从磁盘重载"，由 Game 执行。
    void draw(bool* reload_requested);

private:
    void draw_canvas();
    void draw_map_panel();
    void draw_asset_panel();
    void draw_status_bar();

    void rebuild_filter();
    void switch_layer(int layer);
    void save();
    void set_status(const std::string& s);

    world::EditSession*  session_ = nullptr;
    world::TileMap*      map_  = nullptr;
    world::PropSet*      props_ = nullptr;
    world::AssetBank*    bank_ = nullptr;
    world::Camera*       cam_  = nullptr;
    platform::TextureCache* tex_ = nullptr;
    std::string*         map_path_ = nullptr;

    int  fb_w_ = 640;
    int  fb_h_ = 360;
    bool active_ = false;

    int  layer_ = 0;              // 只是 UI 的镜像，真正改的是 session
    int  brush_size_ = 1;
    int  asset_cat_ = 0;          // 分类下拉
    int  page_ = 0;
    char search_[48] = {};

    bool placing_ = false;
    bool reload_clicked_ = false;

    std::vector<std::string> cats_;
    std::vector<int>         view_;
    bool filter_dirty_ = true;

    std::string status_;
    float status_t_ = 0.f;

    int cells_per_page() const { return 24; }
};

} // namespace ui
