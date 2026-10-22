#pragma once
#include "platform/Window.h"
#include "platform/ImGuiLayer.h"
#include "gfx/Surface.h"
#include "gfx/Atlas.h"
#include "gfx/Blitter.h"
#include "gfx/Ui.h"
#include "ecs/Registry.h"
#include "world/TileMap.h"
#include "world/Props.h"
#include "world/AssetBank.h"
#include "world/EditSession.h"
#include "world/Camera.h"
#include "world/Weather.h"
#include "game/Player.h"
#include "game/Components.h"   // on_enemy_death 用到 Transform
#include "game/Dialog.h"
#include "game/Quest.h"
#include "game/Inventory.h"
#include "platform/TextureCache.h"
#include "ui/EditorUi.h"

#include <string>
#include <vector>

namespace game {

enum class State { Playing, Dialog, GameOver, Ending };
enum class UiStyle { Medieval, SciFi, Wild, Prehistoric };

// ---------------------------------------------------------------
// 地图编辑器
//
// 它不是"另一个程序"，就是 Game 的一个模式：按 F1 切进来，
// 复用同一套 Surface / Blitter / TileMap。好处是所见即所得 ——
// 编辑时看到的就是游戏里跑起来的样子。
//
// 进编辑模式后游戏逻辑暂停（敌人不动、不挨打），
// 只保留相机操作和鼠标绘制。
// ---------------------------------------------------------------
struct EditorState {
    bool  active      = false;
    int   brush       = 17;       // 当前 tile 索引
    bool  solid_mode  = false;    // false=画地形 / true=画碰撞
    int   tileset_idx = 0;        // 在 kEditorTilesets 里的下标
    bool  dragging    = false;    // 左键按住拖动中
    bool  dirty       = false;    // 有未保存的改动
    char  status[96]  = {};       // 状态栏文字
    float status_t    = 0.f;      // 剩余显示秒数
};

// 按 Tab 循环的图集。
// 最后一张在 backgrounds/ 目录（rift 用的 ninja 图集），
// set_tileset 两个目录都会找。
inline constexpr const char* kEditorTilesets[] = {
    "terrain_spring.png",
    "terrain_summer.png",
    "terrain_autumn.png",
    "terrain_winter.png",
    "terrain_winter_ice.png",
    "0-tileset-32x32.png",
};
inline constexpr int kEditorTilesetCount =
    int(sizeof(kEditorTilesets) / sizeof(kEditorTilesets[0]));

class Game {
public:
    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // max_frames > 0 时跑固定帧数后退出（无头自检用）
    void run(int max_frames = -1);

    // ---- 调试接口（截图 / 跳图）----
    // 把当前 framebuffer 存成 PNG。截图、做宣传图、写文档都用它。
    bool save_png(const char* path);

    // 截整个窗口（含 ImGui）。和 save_png 的区别：那个只截 640×360 的游戏内画面。
    // 截图请求在下一帧画完、present 之前执行（backbuffer 那时才有内容）。
    void request_capture(const char* path) { capture_path_ = path; }
    bool save_screen(const char* path);
    // 直接跳到某张图的某个格子（跳过走路）
    void warp(const std::string& map, int tx, int ty);

    // 直接进地图编辑器（正常是游戏里按 F1）。
    // 单独开一个公开方法，是为了让 --edit-shot 能截编辑器的图 ——
    // 截图模式下没法按 F1。
    void enter_editor();

    // 直接进 ImGui 实时编辑器（正常是游戏里按 F2）。
    // --ui-shot 截图模式用。
    void enter_studio();

    // 从磁盘重载当前 .map，保住玩家的位置和血量。
    // 编辑器里点"Reload map"时调 —— 这就是"热加载回游戏"。
    void reload_map_live();

private:
    void load_resources();
    void load_map(const std::string& name, int sx, int sy);

    void handle_input(float dt);
    void update(float dt);
    void render();

    void update_player(float dt);
    void update_enemies(float dt);
    void update_projectiles(float dt);
    void update_pickups(float dt);
    void update_fx(float dt);
    void update_weather(float dt);
    void update_dialog(float dt);
    void check_interactions(float dt);

    void on_enemy_death(ecs::Entity e, Transform& t);
    void spawn_npc(const char* kind, float x, float y);

    // ---- 场景交互（宝箱 / 路牌 / 篝火）----
    // 每帧扫一次玩家周围 2 格，把最近的可交互物记在 near_* 里。
    // 好处是"按 E"和"显示提示"用同一份结果，不用扫两遍。
    void scan_nearby_decor();
    bool try_decor_interact();

    // ---- 地图编辑器 ----
    void toggle_editor();
    void editor_update(float dt);
    void editor_draw();
    void save_current_map();
    void set_status(const char* msg);

    // 鼠标坐标：窗口像素 → 内部分辨率。
    // 窗口 1280×720 显示 640×360，这里除 2；改了分辨率不用动别处。
    int mouse_ix() const;
    int mouse_iy() const;

    void draw_world();
    void draw_parallax();
    void draw_entities();
    void draw_projectiles();
    void draw_pickups();
    void draw_fx();
    void draw_weather();
    void draw_ui();

    // ---- 子系统 ----
    // 析构顺序是声明的反序：texcache_ / ui_ 必须比 win_ 先没，
    // 因为纹理和 ImGui 的后端都依赖 SDL_Renderer。
    platform::Window      win_;
    gfx::Surface          fb_;
    world::AssetBank      bank_;       // 1195 张素材的目录 + 热重载
    world::TileMap        map_;
    world::PropSet        props_;      // 树、木桶、墓碑、篝火……
    world::Camera         cam_;
    world::ParticleSystem particles_;
    ecs::Registry         reg_;

    Player       player_;
    DialogState  dialog_;
    QuestFlags   quest_;
    Inventory    inv_;

    State   state_ = State::Playing;
    UiStyle ui_style_ = UiStyle::Medieval;

    EditorState ed_;
    std::string current_map_path_;   // 保存时写回哪个文件

    // ---- 资产编辑器（F2）----
    // 三层，依赖只朝下：
    //   Game  ──> ui::EditorUi（面板）
    //         ──> world::EditSession（纯逻辑：画笔/撤销/保存）
    //         ──> platform::TextureCache（Surface→纹理）
    platform::ImGuiLayer  ui_;
    platform::TextureCache texcache_;
    world::EditSession    session_;
    ui::EditorUi          editor_;
    bool                 reload_map_ = false;   // 编辑器点了 Reload，下一帧执行
    std::string          capture_path_;         // 非空 = 每帧截一张窗口图

    // 玩家附近最近的可交互装饰（-1 = 没有）
    int near_x_   = -1;
    int near_y_   = -1;
    int near_pid_ = 0;

    float time_       = 0.f;
    float ending_t_   = 0.f;
    float gameover_t_ = 0.f;
    float portal_cd_  = 0.f;

    std::string current_map_;
    const char* blocked_hint_ = nullptr;
    std::string hint_buf_;            // blocked_hint_ 指向这里，防止悬空
    float       blocked_t_    = 0.f;

    // ---- 贴图 ----
    gfx::Surface tex_hero_walk_;
    gfx::Surface tex_ui_panel_;
    gfx::Surface tex_heart_full_;
    gfx::Surface tex_heart_empty_;
    gfx::Surface tex_bar_start_;
    gfx::Surface tex_bar_mid_;
    gfx::Surface tex_bar_end_;
    gfx::Surface tex_energy_mid_;
    gfx::Surface tex_font_;
    gfx::Surface tex_bg_;
    bool         bg_loaded_ = false;

    gfx::BitmapFont font_;

    std::vector<Line> active_lines_;
};

} // namespace game
