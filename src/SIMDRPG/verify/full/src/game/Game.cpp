#include "game/Game.h"

#include "game/Components.h"
#include "game/Combat.h"
#include "game/Enemy.h"
#include "platform/Input.h"
#include "simd/Backend.h"
#include "world/Collision.h"

#include <SDL.h>
#include <SDL_image.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>

namespace game {

namespace {

constexpr int INT_W = 640;
constexpr int INT_H = 360;
constexpr int WIN_W = 1280;
constexpr int WIN_H = 720;

// 全局只读资源池（掉落 / 特效用）
std::vector<gfx::Surface> g_items;
std::vector<gfx::Surface> g_fx;

} // namespace

// ---------------------------------------------------------------
Game::Game()
    : win_(WIN_W, WIN_H, INT_W, INT_H, "Embers of Eldra")
    , fb_(INT_W, INT_H)
    , font_{}
{
    std::printf("[simd] backend = %s\n", simd::isa_name(simd::active()));

    // 先扫目录（只读文件名，不解码），道具表再从里面取图
    bank_.set_root("assets/gfx");
    bank_.scan();

    load_resources();
    load_map("village", 32, 20);

    // ImGui 必须在 Window 建好之后、第一帧之前初始化
    ui_.init(win_);
    texcache_.init(static_cast<SDL_Renderer*>(win_.native_renderer()));

    // 编辑器拆成两层：逻辑（EditSession）和界面（EditorUi）。
    // 这里把"谁的指针交给谁"一次接好，后面两边都不再互相找。
    session_.bind(&map_, &props_, &bank_, &current_map_path_);
    editor_.init(&session_, &map_, &props_, &bank_, &cam_, &texcache_,
                 INT_W, INT_H, &current_map_path_);
}

Game::~Game() {
    // 顺序：先放纹理，再拆 ImGui，最后才轮到 Window（成员按声明反序析构）
    texcache_.shutdown();
    ui_.shutdown();
}

// ---------------------------------------------------------------
void Game::load_resources() {
    const std::string base = "assets/gfx/";

    tex_hero_walk_   = gfx::load_png(base + "chars/hero_walk.png");
    tex_ui_panel_    = gfx::load_png(base + "ui/box.png");
    tex_heart_full_  = gfx::load_png(base + "ui/full-heart.png");
    tex_heart_empty_ = gfx::load_png(base + "ui/empty-heart.png");
    tex_bar_start_   = gfx::load_png(base + "ui/bar-start.png");
    tex_bar_mid_     = gfx::load_png(base + "ui/bar-middle.png");
    tex_bar_end_     = gfx::load_png(base + "ui/bar-end.png");
    tex_energy_mid_  = gfx::load_png(base + "ui/energy-bar.png");
    props_.load(bank_, "assets/props.cfg");

    tex_font_        = gfx::load_png(base + "fonts/font8x8.png");

    font_.sheet = &tex_font_;
    font_.cw = 8;
    font_.ch = 8;
    font_.cols = 15;

    std::printf("[game] 加载道具池...\n");
    g_items = gfx::load_dir(base + "objects");
    std::printf("[game] 加载特效池...\n");
    g_fx    = gfx::load_dir(base + "fx");
    std::printf("[game] 道具 %zu 个, 特效 %zu 个\n", g_items.size(), g_fx.size());

    // 注意：不要再写 items_ = g_items。
    // Surface 禁拷贝，vector<Surface> 的拷贝赋值编译不过（坑 7）。
    // 直接用全局池就行，不需要第二份。
}

// ---------------------------------------------------------------
void Game::load_map(const std::string& name, int sx, int sy) {
    std::printf("[game] 载入地图 %s\n", name.c_str());

    current_map_path_ = "assets/maps/" + name + ".map";
    map_ = world::TileMap::load(current_map_path_);
    current_map_ = name;

    reg_.clear();

    // 生成实体
    for (const auto& s : map_.spawns) {
        if (s.type.rfind("enemy_", 0) == 0) {
            spawn_enemy(reg_, s.type.c_str() + 6,
                        float(s.x * world::TileMap::TS),
                        float(s.y * world::TileMap::TS));
        } else if (s.type.rfind("boss_", 0) == 0) {
            spawn_enemy(reg_, s.type.c_str(),
                        float(s.x * world::TileMap::TS),
                        float(s.y * world::TileMap::TS));
        } else if (s.type.rfind("npc_", 0) == 0) {
            spawn_npc(s.type.c_str() + 4,          // "npc_elder" -> "elder"
                      float(s.x * world::TileMap::TS),
                      float(s.y * world::TileMap::TS));
        }
    }

    // 玩家
    player_.x  = float(sx * world::TileMap::TS);
    player_.y  = float(sy * world::TileMap::TS);
    player_.hp = player_.max_hp;
    player_.invuln = 0.f;
    player_.attack_cd = 0.f;
    player_.attack.active = false;
    player_.anim.setup(&tex_hero_walk_, 64, 64, 8, 4);

    // 天气
    particles_.count = 0;
    const world::WeatherKind wk = world::from_string(map_.weather.c_str());
    if (wk == world::WeatherKind::Rain)
        world::reset_rain(particles_, 2000, fb_.width(), fb_.height());
    else if (wk == world::WeatherKind::Snow)
        world::reset_snow(particles_, 3000, fb_.width(), fb_.height());
    else if (wk == world::WeatherKind::Meteor)
        world::reset_meteor(particles_, 80, fb_.width(), fb_.height());

    // 背景
    bg_loaded_ = false;
    if (!map_.bg.empty() && map_.bg != "none") {
        tex_bg_ = gfx::load_png("assets/gfx/backgrounds/" + map_.bg);
        bg_loaded_ = true;
    }

    // UI 风格
    if (name == "rift")      ui_style_ = UiStyle::SciFi;
    else if (name == "nest") ui_style_ = UiStyle::Prehistoric;
    else                     ui_style_ = UiStyle::Medieval;

    // 剧情：进山道就找到霜之泪
    if (name == "pass" && !quest_.frost_tear) {
        quest_.frost_tear = true;
        std::printf("[quest] 找到霜之泪\n");
    }

    cam_.follow(int(player_.x), int(player_.y), fb_.width(), fb_.height(),
                map_.pixel_width(), map_.pixel_height());

    portal_cd_ = 0.5f;      // 防止在传送门上反复横跳
}

// ---------------------------------------------------------------
void Game::run(int max_frames) {
    using clock = std::chrono::steady_clock;
    auto last = clock::now();

    int frame = 0;
    while (win_.poll()) {
        platform::Input::begin_frame();

        const auto now = clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;
        if (dt > 1.f / 30.f) dt = 1.f / 30.f;

        ui_.begin_frame();

        handle_input(dt);
        update(dt);
        render();

        // 游戏画面先铺上去，ImGui 再盖在上面，最后才 Present
        win_.begin_scene(fb_.data(), fb_.pitch());
        ui_.end_frame();

        // 截图要在 present 之前读 backbuffer
        if (!capture_path_.empty()) save_screen(capture_path_.c_str());

        win_.present();

        platform::Input::end_frame();

        if (max_frames > 0 && ++frame >= max_frames) break;
    }
    std::printf("[game] 退出（跑了 %d 帧）\n", frame);
}

// ---------------------------------------------------------------
void Game::warp(const std::string& map, int tx, int ty) {
    load_map(map, tx, ty);
}

void Game::enter_editor() {
    if (!ed_.active) toggle_editor();
}

void Game::enter_studio() {
    if (ed_.active) toggle_editor();
    editor_.set_active(true);
}

void Game::reload_map_live() {
    const int px = int(player_.x);
    const int py = int(player_.y);
    const int hp = player_.hp;

    std::printf("[game] 热加载地图 %s\n", current_map_path_.c_str());
    load_map(current_map_, px / world::TileMap::TS, py / world::TileMap::TS);

    // load_map 会把玩家放回出生点、血补满。热加载不该改变玩家状态，
    // 所以这里再盖回去。
    player_.x  = float(px);
    player_.y  = float(py);
    player_.hp = hp;
    cam_.follow(px, py, fb_.width(), fb_.height(),
                map_.pixel_width(), map_.pixel_height());
}

// ---------------------------------------------------------------
// 地图编辑器
// ---------------------------------------------------------------
int Game::mouse_ix() const {
    const int ww = win_.window_width();
    if (ww <= 0) return platform::Input::mouse_x();
    return int(float(platform::Input::mouse_x()) * float(fb_.width()) / float(ww));
}

int Game::mouse_iy() const {
    const int wh = win_.window_height();
    if (wh <= 0) return platform::Input::mouse_y();
    return int(float(platform::Input::mouse_y()) * float(fb_.height()) / float(wh));
}

void Game::set_status(const char* msg) {
    std::snprintf(ed_.status, sizeof(ed_.status), "%s", msg);
    ed_.status_t = 3.f;
    std::printf("[edit] %s\n", msg);       // 同时打到控制台，方便无头验证
}

void Game::toggle_editor() {
    ed_.active = !ed_.active;

    if (!ed_.active) {
        std::printf("[edit] 退出编辑模式%s\n", ed_.dirty ? "（有未保存改动）" : "");
        return;
    }

    // 记住当前图集在列表里的位置，Tab 才能从"现在这张"往后轮
    ed_.tileset_idx = 0;
    for (int i = 0; i < kEditorTilesetCount; ++i) {
        if (map_.tileset == kEditorTilesets[i]) { ed_.tileset_idx = i; break; }
    }

    const int max_tile = std::max(0, map_.tile_count() - 1);
    ed_.brush = std::min(17, max_tile);       // 17 = LPC 图集的纯地面

    std::printf("[edit] 进入编辑模式 %s（%s, tpr=%d, %d 个 tile）\n",
                current_map_.c_str(), map_.tileset.c_str(),
                map_.tpr, map_.tile_count());
    set_status("LMB paint / RMB erase / MMB pick / Tab tileset / T mode / F5 save / F1 exit");
}

void Game::save_current_map() {
    if (current_map_path_.empty()) {
        set_status("没有地图路径，无法保存");
        return;
    }
    if (map_.save(current_map_path_)) {
        ed_.dirty = false;
        char buf[96];
        std::snprintf(buf, sizeof(buf), "已保存 %s", current_map_path_.c_str());
        set_status(buf);
    } else {
        set_status("保存失败（看控制台）");
    }
}

void Game::editor_update(float dt) {
    if (ed_.status_t > 0.f) {
        ed_.status_t -= dt;
        if (ed_.status_t < 0.f) ed_.status_t = 0.f;
    }

    // ---- 相机平移 ----
    // 编辑时相机不再跟着玩家，要自己挪。速度比角色快，方便扫整张图。
    const int v = int(480.f * dt);
    if (platform::Input::down(SDL_SCANCODE_A) || platform::Input::down(SDL_SCANCODE_LEFT))  cam_.x -= v;
    if (platform::Input::down(SDL_SCANCODE_D) || platform::Input::down(SDL_SCANCODE_RIGHT)) cam_.x += v;
    if (platform::Input::down(SDL_SCANCODE_W) || platform::Input::down(SDL_SCANCODE_UP))    cam_.y -= v;
    if (platform::Input::down(SDL_SCANCODE_S) || platform::Input::down(SDL_SCANCODE_DOWN))  cam_.y += v;

    // 夹在地图范围内，别让相机跑到图外面（那时画出来全是清屏色，会以为图坏了）
    const int max_cx = std::max(0, map_.pixel_width()  - fb_.width());
    const int max_cy = std::max(0, map_.pixel_height() - fb_.height());
    cam_.x = std::clamp(cam_.x, 0, max_cx);
    cam_.y = std::clamp(cam_.y, 0, max_cy);
    cam_.shake_time = 0.f;                    // 震屏会干扰对齐

    const int max_tile = std::max(0, map_.tile_count() - 1);

    // ---- 换画笔：滚轮 ±1，Q/E 也行（笔记本触摸板没滚轮）----
    int d = platform::Input::wheel();
    if (platform::Input::pressed(SDL_SCANCODE_E)) ++d;
    if (platform::Input::pressed(SDL_SCANCODE_Q)) --d;
    if (d != 0) {
        ed_.brush = std::clamp(ed_.brush + d, 0, max_tile);
        char buf[64];
        std::snprintf(buf, sizeof(buf), "tile %d / %d", ed_.brush, max_tile);
        set_status(buf);
    }

    // ---- 换图集（Tab 循环四季）----
    if (platform::Input::pressed(SDL_SCANCODE_TAB)) {
        const int next = (ed_.tileset_idx + 1) % kEditorTilesetCount;
        if (map_.set_tileset(kEditorTilesets[next])) {
            ed_.tileset_idx = next;

            // 关键：换了图集，原来的索引可能整片越界。
            // 不夹的话 draw_world 会去图集外面取样，画出一堆黑块。
            const int last = std::max(0, map_.tile_count() - 1);
            int fixed = 0;
            for (int& g : map_.ground) {
                if (g > last) { g = last; ++fixed; }
            }
            ed_.brush = std::min(ed_.brush, last);
            ed_.dirty = true;

            char buf[112];
            std::snprintf(buf, sizeof(buf), "%s（%d 个 tile，%d 格越界已修正）",
                          map_.tileset.c_str(), map_.tile_count(), fixed);
            set_status(buf);
        }
    }

    // ---- 切画笔类型：地形 / 碰撞 ----
    if (platform::Input::pressed(SDL_SCANCODE_T)) {
        ed_.solid_mode = !ed_.solid_mode;
        set_status(ed_.solid_mode ? "模式：碰撞（左键刷墙，右键擦墙）"
                                  : "模式：地形（左键画，右键擦回 17）");
    }

    // ---- 保存 ----
    if (platform::Input::pressed(SDL_SCANCODE_F5)) save_current_map();

    // ---- 鼠标 ----
    const int mx = mouse_ix();
    const int my = mouse_iy();

    // 调色板：左上角，每格 12px。先做命中测试，
    // 否则点调色板会顺手把那一格地图也画了。
    const int cell = 12;
    const int pal_w = map_.tpr * cell;
    const int rows  = (map_.tpr > 0) ? (map_.tile_count() + map_.tpr - 1) / map_.tpr : 0;
    const int pal_h = rows * cell;
    const bool in_palette = (mx >= 0 && my >= 0 && mx < pal_w && my < pal_h);

    if (in_palette) {
        if (platform::Input::mouse_pressed(1)) {
            const int idx = (my / cell) * map_.tpr + (mx / cell);
            if (idx >= 0 && idx < map_.tile_count()) {
                ed_.brush = idx;
                char buf[48];
                std::snprintf(buf, sizeof(buf), "tile %d", idx);
                set_status(buf);
            }
        }
        return;                       // 在调色板里就只选色，不画地图
    }

    // 屏幕坐标 → 格子坐标
    const int tx = (cam_.x + mx) / world::TileMap::TS;
    const int ty = (cam_.y + my) / world::TileMap::TS;
    if (tx < 0 || ty < 0 || tx >= map_.w || ty >= map_.h) return;

    if (platform::Input::mouse_down(1)) {
        // 左键：画
        if (ed_.solid_mode) {
            map_.set_solid(tx, ty, true);
            ed_.dirty = true;
        } else if (map_.at(tx, ty) != ed_.brush) {
            map_.paint(tx, ty, ed_.brush);
            ed_.dirty = true;
        }
    } else if (platform::Input::mouse_down(3)) {
        // 右键：擦
        if (ed_.solid_mode) {
            map_.set_solid(tx, ty, false);
        } else if (map_.at(tx, ty) != 17) {
            map_.paint(tx, ty, 17);       // 17 = 纯地面
        }
        ed_.dirty = true;
    } else if (platform::Input::mouse_pressed(2)) {
        // 中键：吸管（把光标下的地形捡起来当画笔）
        ed_.brush = map_.at(tx, ty);
        char buf[64];
        std::snprintf(buf, sizeof(buf), "picked tile %d", ed_.brush);
        set_status(buf);
    }
}

void Game::editor_draw() {
    const int ts = world::TileMap::TS;
    const int cx = cam_.x;
    const int cy = cam_.y;

    // 1) 地形。故意不画视差背景和天气 —— 编辑时要看得越干净越好。
    draw_world();

    // 2) 碰撞层：solid=1 的格子叠一层红。
    //    只遍历可见区！全图 64×48 = 3072 格、每格 1024 像素，
    //    每帧混 300 万次会让编辑器卡成 5 帧。
    const int x0 = std::max(0, cx / ts);
    const int y0 = std::max(0, cy / ts);
    const int x1 = std::min(map_.w - 1, (cx + fb_.width())  / ts + 1);
    const int y1 = std::min(map_.h - 1, (cy + fb_.height()) / ts + 1);
    for (int ty = y0; ty <= y1; ++ty)
        for (int tx = x0; tx <= x1; ++tx)
            if (map_.is_solid(tx, ty))
                gfx::fill_rect_alpha(fb_, { tx * ts - cx, ty * ts - cy, ts, ts },
                                     0x50FF2020u);

    // 3) 实体标记：红=敌人、绿=NPC、蓝=传送门。
    //    编辑时看不到实体，但得知道它们的坐标，不然会把怪埋进墙里。
    for (const auto& sp : map_.spawns) {
        core::Pixel c = 0xFFFF4040u;
        if (sp.type.rfind("npc_", 0) == 0)      c = 0xFF40FF40u;
        else if (sp.type == "portal")            c = 0xFF4090FFu;
        gfx::fill_rect(fb_, { sp.x * ts - cx + 8, sp.y * ts - cy + 8, 16, 16 }, c);
    }

    // 4) 网格线
    for (int x = -(cx % ts); x < fb_.width(); x += ts)
        gfx::fill_rect_alpha(fb_, { x, 0, 1, fb_.height() }, 0x28FFFFFFu);
    for (int y = -(cy % ts); y < fb_.height(); y += ts)
        gfx::fill_rect_alpha(fb_, { 0, y, fb_.width(), 1 }, 0x28FFFFFFu);

    // 5) 鼠标所在格：黄色高亮框
    const int hx = ((cx + mouse_ix()) / ts) * ts - cx;
    const int hy = ((cy + mouse_iy()) / ts) * ts - cy;
    gfx::fill_rect(fb_, { hx, hy,             ts, 1  }, 0xFFFFFF00u);
    gfx::fill_rect(fb_, { hx, hy + ts - 1,    ts, 1  }, 0xFFFFFF00u);
    gfx::fill_rect(fb_, { hx, hy,             1,  ts }, 0xFFFFFF00u);
    gfx::fill_rect(fb_, { hx + ts - 1, hy,    1,  ts }, 0xFFFFFF00u);

    // 6) 调色板（左上角）
    const int cell  = 12;
    const int pal_w = map_.tpr * cell;
    const int rows  = (map_.tpr > 0) ? (map_.tile_count() + map_.tpr - 1) / map_.tpr : 0;
    const int pal_h = rows * cell;

    gfx::fill_rect(fb_, { 0, 0, pal_w, pal_h }, 0xF0101018u);
    for (int i = 0; i < map_.tile_count(); ++i) {
        const int c = i % map_.tpr;
        const int r = i / map_.tpr;
        gfx::blit_stretch(fb_, map_.image, map_.tile_rect(i),
                          { c * cell, r * cell, cell, cell });
    }

    // 选中框
    const int bx = (ed_.brush % map_.tpr) * cell;
    const int by = (ed_.brush / map_.tpr) * cell;
    gfx::fill_rect(fb_, { bx,             by,             cell, 2    }, 0xFFFFFFFFu);
    gfx::fill_rect(fb_, { bx,             by + cell - 2,  cell, 2    }, 0xFFFFFFFFu);
    gfx::fill_rect(fb_, { bx,             by,             2,    cell }, 0xFFFFFFFFu);
    gfx::fill_rect(fb_, { bx + cell - 2,  by,             2,    cell }, 0xFFFFFFFFu);

    // 7) 底部状态栏
    const int bar_h = 18;
    const int bar_y = fb_.height() - bar_h;
    gfx::fill_rect(fb_, { 0, bar_y, fb_.width(), bar_h }, 0xF0181010u);

    char buf[160];
    std::snprintf(buf, sizeof(buf), "%s  %s  tile %d/%d  %s%s",
                  map_.name.c_str(), map_.tileset.c_str(),
                  ed_.brush, std::max(0, map_.tile_count() - 1),
                  ed_.solid_mode ? "SOLID" : "GROUND",
                  ed_.dirty ? "  *" : "");
    gfx::draw_text(fb_, font_, 4, bar_y + 5, buf, 0xFFE8E8E8u);

    // 8) 临时提示（3 秒）
    if (ed_.status_t > 0.f && ed_.status[0]) {
        gfx::fill_rect_alpha(fb_, { 0, bar_y - 14, fb_.width(), 14 }, 0xB0000000u);
        gfx::draw_text(fb_, font_, 4, bar_y - 12, ed_.status, 0xFFFFE080u);
    }
}

bool Game::save_screen(const char* path) {
    gfx::Surface shot(win_.window_width(), win_.window_height());
    if (!win_.read_pixels(shot.data(), shot.pitch())) {
        std::fprintf(stderr, "截窗口失败: %s\n", SDL_GetError());
        return false;
    }
    SDL_Surface* view = SDL_CreateRGBSurfaceFrom(
        shot.data(), shot.width(), shot.height(), 32,
        shot.pitch() * int(sizeof(core::Pixel)),
        0x00FF0000u, 0x0000FF00u, 0x000000FFu, 0xFF000000u);
    if (!view) return false;
    const int rc = IMG_SavePNG(view, path);
    SDL_FreeSurface(view);
    if (rc != 0) {
        std::fprintf(stderr, "截窗口失败(编码): %s\n", IMG_GetError());
        return false;
    }
    std::printf("[shot] 已保存 %s (%dx%d，含 UI)\n", path, shot.width(), shot.height());
    return true;
}

bool Game::save_png(const char* path) {
    // 把 Surface 包成一个 SDL_Surface 视图（不拷贝像素），交给 SDL_image 编码
    SDL_Surface* view = SDL_CreateRGBSurfaceFrom(
        fb_.data(), fb_.width(), fb_.height(), 32,
        fb_.pitch() * int(sizeof(core::Pixel)),
        0x00FF0000u, 0x0000FF00u, 0x000000FFu, 0xFF000000u);
    if (!view) {
        std::fprintf(stderr, "截图失败(创建视图): %s\n", SDL_GetError());
        return false;
    }
    const int rc = IMG_SavePNG(view, path);
    SDL_FreeSurface(view);          // 只释放视图，像素还是 fb_ 的
    if (rc != 0) {
        std::fprintf(stderr, "截图失败(编码): %s\n", IMG_GetError());
        return false;
    }
    std::printf("[shot] 已保存 %s (%dx%d)\n", path, fb_.width(), fb_.height());
    return true;
}

// ---------------------------------------------------------------
void Game::handle_input(float dt) {
    (void)dt;

    // F2 进出 ImGui 实时编辑器。和 F1 手搓版互斥。
    if (platform::Input::pressed(SDL_SCANCODE_F2)) {
        if (editor_.active()) {
            editor_.set_active(false);
        } else {
            if (ed_.active) toggle_editor();
            editor_.set_active(true);
        }
    }
    if (editor_.active()) return;      // 编辑器打开时，游戏输入全部让路

    // F1 进出地图编辑器。放在最前面 —— 任何状态（对话/结算）都能切。
    if (platform::Input::pressed(SDL_SCANCODE_F1)) {
        toggle_editor();
        return;
    }
    if (ed_.active) return;        // 编辑模式的操作在 editor_update 里

    if (state_ == State::Dialog) {
        if (platform::Input::pressed(SDL_SCANCODE_E) ||
            platform::Input::pressed(SDL_SCANCODE_SPACE) ||
            platform::Input::pressed(SDL_SCANCODE_RETURN)) {
            if (dialog_.advance()) state_ = State::Playing;
        }
        return;
    }

    if (state_ != State::Playing) return;

    int dx = 0, dy = 0;
    if (platform::Input::down(SDL_SCANCODE_A) || platform::Input::down(SDL_SCANCODE_LEFT))  dx -= 1;
    if (platform::Input::down(SDL_SCANCODE_D) || platform::Input::down(SDL_SCANCODE_RIGHT)) dx += 1;
    if (platform::Input::down(SDL_SCANCODE_W) || platform::Input::down(SDL_SCANCODE_UP))    dy -= 1;
    if (platform::Input::down(SDL_SCANCODE_S) || platform::Input::down(SDL_SCANCODE_DOWN))  dy += 1;

    player_.move_dir_x = dx;
    player_.move_dir_y = dy;

    if (platform::Input::down(SDL_SCANCODE_J) || platform::Input::down(SDL_SCANCODE_SPACE))
        player_.want_attack = true;

    if (platform::Input::pressed(SDL_SCANCODE_E))
        player_.want_interact = true;

    if (platform::Input::pressed(SDL_SCANCODE_Q))
        inv_.next_weapon(WEAPON_COUNT);
}

// ---------------------------------------------------------------
void Game::update_player(float dt) {
    // 移动
    if (player_.move_dir_x != 0 || player_.move_dir_y != 0) {
        // 斜着走不能比直着快：两个分量都乘 1/√2
        const bool diag = (player_.move_dir_x != 0 && player_.move_dir_y != 0);
        const float k = diag ? 0.70711f : 1.f;

        const float vx = float(player_.move_dir_x) * player_.speed * dt * k;
        const float vy = float(player_.move_dir_y) * player_.speed * dt * k;
        world::try_move(player_.x, player_.y, vx, vy, map_, false);
    }

    player_.update(dt);

    // 起手
    if (player_.want_attack) {
        player_.want_attack = false;
        if (player_.attack_cd <= 0.f) {
            const Weapon& w = WEAPONS[inv_.weapon_index];

            if (w.kind == Weapon::Kind::Melee) {
                player_.attack_cd = w.cooldown;
                player_.attack.trigger(player_.anim.row);
            } else {
                const bool has_ammo = (w.ammo < 0) || (inv_.ammo > 0);
                if (has_ammo) {
                    player_.attack_cd = w.cooldown;
                    if (w.ammo >= 0) --inv_.ammo;

                    float dx = 0.f, dy = 0.f;
                    switch (player_.anim.row) {
                        case 0: dy = -1.f; break;
                        case 1: dx = -1.f; break;
                        case 3: dx =  1.f; break;
                        default: dy = 1.f; break;
                    }
                    spawn_projectile(reg_, player_.x + 28.f, player_.y + 28.f,
                                     dx * 420.f, dy * 420.f,
                                     w.damage, true, w.pierce);
                }
            }
        }
    }

    // 近战判定
    if (player_.attack.active && !player_.attack.hit_done) {
        const Weapon& w = WEAPONS[inv_.weapon_index];
        if (w.kind == Weapon::Kind::Melee) {
            const core::Rect box = melee_box(player_.x, player_.y,
                                             player_.attack.dir, w.range);
            bool hit_any = false;

            reg_.each<Transform, Health>([&](ecs::Entity, Transform& t, Health& h) {
                if (h.hp <= 0) return;
                const core::Rect body{ int(t.x) + 16, int(t.y) + 40, 32, 20 };
                if (!core::rect_overlap(box, body)) return;

                h.hp -= w.damage;
                h.flash = 0.12f;
                hit_any = true;
            });

            if (hit_any) {
                player_.attack.hit_done = true;
                cam_.shake(2.f, 0.08f);
            }
        }
    }
}

// ---------------------------------------------------------------
void Game::update_enemies(float dt) {
    reg_.each<Transform, Health, AI>(
        [&](ecs::Entity e, Transform& t, Health& h, AI& ai) {
            if (h.hp <= 0) {
                on_enemy_death(e, t);
                return;
            }
            if (ai.stun > 0.f) { ai.stun -= dt; return; }

            const float dx = player_.x - t.x;
            const float dy = player_.y - t.y;
            const float d  = std::sqrt(dx * dx + dy * dy) + 1e-5f;

            // 视觉朝向：4 行图集才需要
            if (Sprite* sp = reg_.get<Sprite>(e)) {
                if (sp->anim.rows >= 4) {
                    if      (std::abs(dy) > std::abs(dx)) sp->anim.row = (dy < 0) ? 0 : 2;
                    else if (dx < 0)                      sp->anim.row = 1;
                    else                                  sp->anim.row = 3;
                }
                sp->anim.update(dt, true, 0, 0);
            }

            // Boss 阶段
            if (Boss* b = reg_.get<Boss>(e)) {
                const float ratio = float(h.hp) / float(h.max_hp);
                const int want = (ratio > 0.66f) ? 1 : (ratio > 0.33f ? 2 : 3);
                if (want != b->phase) {
                    b->phase = want;
                    cam_.shake(7.f, 0.45f);
                    std::printf("[boss] 进入第 %d 阶段\n", want);
                }
                const float k = (b->phase == 1) ? 1.f : (b->phase == 2 ? 1.3f : 1.6f);
                ai.speed = 45.f * k;
            }

            if (ai.behavior & B::Static) {
                // 站桩，但仍然会远程
            } else if (d < float(ai.sight) && (ai.behavior & B::Chase)) {
                float sp = ai.speed;
                if ((ai.behavior & B::Charge) && d > 60.f && d < 260.f) sp *= 1.8f;

                const float stop = (ai.behavior & B::Ranged) ? 70.f : 0.f;
                if (d > stop) {
                    world::try_move(t.x, t.y,
                                    dx / d * sp * dt,
                                    dy / d * sp * dt,
                                    map_,
                                    (ai.behavior & B::Flying) != 0);
                }
            }

            // 远程开火
            if ((ai.behavior & B::Ranged) && ai.ranged_range > 0 &&
                d < float(ai.ranged_range)) {
                if (ai.attack_cd <= 0.f) {
                    ai.attack_cd = (reg_.has<Boss>(e) && reg_.get<Boss>(e)->phase == 3)
                                   ? 0.7f : 1.6f;
                    spawn_projectile(reg_, t.x + 24.f, t.y + 24.f,
                                     dx / d * 240.f, dy / d * 240.f,
                                     6, false, false);
                }
            }
            if (ai.attack_cd > 0.f) ai.attack_cd -= dt;

            // 闪现
            if (ai.behavior & B::Blink) {
                ai.blink_cd += dt;
                if (ai.blink_cd >= 3.f && d < 400.f) {
                    ai.blink_cd = 0.f;
                    const float jx = t.x + dx / d * 80.f;
                    const float jy = t.y + dy / d * 80.f;
                    if (!world::box_solid(world::feet(jx, jy), map_)) {
                        t.x = jx;
                        t.y = jy;
                    }
                }
            }

            // 接触伤害（Boss 更疼）
            const core::Rect body{ int(t.x) + 16, int(t.y) + 40, 32, 20 };
            if (core::rect_overlap(body, world::feet(player_.x, player_.y))) {
                player_.take_damage(reg_.has<Boss>(e) ? 15 : 1);
            }
        });
}

// ---------------------------------------------------------------
void Game::spawn_npc(const char* kind, float x, float y) {
    const char* rel = nullptr;
    if      (std::strcmp(kind, "elder") == 0)    rel = "chars/elder_walk.png";
    else if (std::strcmp(kind, "smith") == 0)    rel = "chars/smith_walk.png";
    else if (std::strcmp(kind, "merchant") == 0) rel = "chars/merchant_walk.png";
    else return;

    const gfx::Surface* sheet = load_sheet(rel);

    const ecs::Entity e = reg_.create();
    reg_.add<Transform>(e, x, y);

    Sprite sp(sheet, 48, 48);
    sp.anim.setup(sheet, 64, 64, 8, 4);
    sp.anim.row = 2;                    // 正面朝玩家
    reg_.add<Sprite>(e, sp);

    // 注意：不加 Health、不加 AI。
    // 所以 update_enemies 的 each<Transform,Health,AI> 不会碰他们，
    // 他们不会动、不会挨打、也不会挡路。
}

// ---------------------------------------------------------------
void Game::on_enemy_death(ecs::Entity e, Transform& t) {
    const EnemyTag* tag = reg_.get<EnemyTag>(e);
    const EnemySpec* spec = tag ? tag->spec : nullptr;

    if (spec) {
        if (spec->split_into) {
            for (int i = 0; i < 2; ++i) {
                spawn_enemy(reg_, spec->split_into,
                            t.x + float(i) * 24.f - 12.f, t.y);
            }
        }
        if (spec->summon_into && (std::rand() % 100) < 40) {
            spawn_enemy(reg_, spec->summon_into, t.x + 40.f, t.y);
        }

        if (spec->is_boss) {
            quest_.boss_defeated = true;
            if (std::strcmp(spec->name, "boss_blight") == 0) quest_.ash_key = true;
            if (std::strcmp(spec->name, "boss_trex")   == 0) quest_.primal_blood = true;
            if (std::strcmp(spec->name, "boss_dragon") == 0) {
                state_ = State::Ending;
                ending_t_ = 0.f;
            }
            std::printf("[game] 击败 %s\n", spec->name);
        }
    }

    // 掉落 + 特效
    drop_item(reg_, t.x, t.y, int(g_items.size()));
    spawn_fx(reg_, t.x, t.y, int(g_fx.size()));

    reg_.destroy(e);        // 永远最后一行
}

// ---------------------------------------------------------------
void Game::update_projectiles(float dt) {
    reg_.each<Transform, Velocity, Projectile>(
        [&](ecs::Entity e, Transform& t, Velocity& v, Projectile& p) {
            t.x += v.x * dt;
            t.y += v.y * dt;
            p.life -= dt;

            if (p.life <= 0.f ||
                t.x < -64.f || t.y < -64.f ||
                t.x > float(map_.pixel_width())  + 64.f ||
                t.y > float(map_.pixel_height()) + 64.f) {
                reg_.destroy(e);
                return;
            }

            const core::Rect box{ int(t.x) - 4, int(t.y) - 3, 8, 6 };

            if (p.from_player) {
                bool hit = false;
                reg_.each<Transform, Health>(
                    [&](ecs::Entity, Transform& ot, Health& oh) {
                        if (hit || oh.hp <= 0) return;
                        const core::Rect body{ int(ot.x) + 16, int(ot.y) + 40, 32, 20 };
                        if (!core::rect_overlap(box, body)) return;
                        oh.hp -= p.damage;
                        oh.flash = 0.12f;
                        hit = true;
                    });
                if (hit && !p.pierce) {
                    reg_.destroy(e);
                    return;
                }
            } else {
                if (core::rect_overlap(box, world::feet(player_.x, player_.y))) {
                    player_.take_damage(p.damage);
                    reg_.destroy(e);
                    return;
                }
            }
        });
}

// ---------------------------------------------------------------
void Game::update_pickups(float dt) {
    (void)dt;
    reg_.each<Transform, Pickup>([&](ecs::Entity e, Transform& t, Pickup& pk) {
        const float ddx = t.x - player_.x;
        const float ddy = t.y - player_.y;
        if (ddx * ddx + ddy * ddy > 40.f * 40.f) return;

        switch (pk.kind) {
            case 0: inv_.gold += 5; break;
            case 1:
                inv_.potions += 1;
                player_.hp = std::min(player_.hp + 20, player_.max_hp);
                break;
            default: inv_.ammo += 6; break;
        }
        reg_.destroy(e);
    });
}

void Game::update_fx(float dt) {
    reg_.each<Transform, Fx>([&](ecs::Entity e, Transform&, Fx& f) {
        f.life -= dt;
        if (f.life <= 0.f) reg_.destroy(e);
    });
}

void Game::update_weather(float dt) {
    if (particles_.count <= 0) return;
    if (world::from_string(map_.weather.c_str()) == world::WeatherKind::Fog) return;
    world::update_particles_simd(particles_, dt, fb_.height());
}

void Game::update_dialog(float dt) {
    dialog_.update(dt);
}

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// 场景交互
// ---------------------------------------------------------------
void Game::scan_nearby_decor() {
    near_x_ = near_y_ = -1;
    near_pid_ = 0;

    const int ts  = world::TileMap::TS;
    const int ptx = (int(player_.x) + 32) / ts;
    const int pty = (int(player_.y) + 32) / ts;

    // 只找"最近的一个"，而不是随便一个 ——
    // 两个宝箱并排时，按 E 要开你正站着的那个。
    float best = 72.f * 72.f;
    for (int oy = -2; oy <= 2; ++oy) {
        for (int ox = -2; ox <= 2; ++ox) {
            const int gx = ptx + ox;
            const int gy = pty + oy;
            const int pid = map_.decor_at(gx, gy);
            // 能不能交互问道具表，不再写死几个常量 ——
            // 这样资产编辑器里勾一下 interactive，新道具立刻能按 E。
            if (pid <= 0 || !props_.interactive(pid - 1)) continue;

            const float cx  = float(gx * ts) + 16.f;
            const float cy  = float(gy * ts) + 16.f;
            const float ddx = cx - (player_.x + 32.f);
            const float ddy = cy - (player_.y + 48.f);
            const float d2  = ddx * ddx + ddy * ddy;
            if (d2 < best) {
                best = d2;
                near_x_ = gx;
                near_y_ = gy;
                near_pid_ = pid;
            }
        }
    }
}

bool Game::try_decor_interact() {
    if (near_x_ < 0 || near_pid_ == 0) return false;

    const float wx = float(near_x_ * world::TileMap::TS);
    const float wy = float(near_y_ * world::TileMap::TS);

    switch (near_pid_) {
        case world::prop_id::Chest: {
            // 箱子连同里面的东西一起拿走：装饰清 0，掉 3 个道具
            map_.paint_decor(near_x_, near_y_, 0);
            inv_.gold += 25;
            for (int i = 0; i < 3; ++i)
                drop_item(reg_, wx + float(i * 10), wy, int(g_items.size()));
            spawn_fx(reg_, wx, wy, int(g_fx.size()));
            blocked_hint_ = "Chest: +25 gold, 3 items";
            blocked_t_ = 2.2f;
            near_x_ = near_y_ = -1;
            near_pid_ = 0;
            return true;
        }

        case world::prop_id::Sign1:
        case world::prop_id::Sign2: {
            active_lines_ = sign_lines(current_map_);
            dialog_.start(active_lines_);
            state_ = State::Dialog;
            return true;
        }

        case world::prop_id::Fire: {
            if (player_.hp >= player_.max_hp) {
                blocked_hint_ = "Already at full health.";
                blocked_t_ = 1.5f;
                return true;
            }
            player_.hp = std::min(player_.max_hp, player_.hp + 30);
            spawn_fx(reg_, wx, wy, int(g_fx.size()));
            blocked_hint_ = "Rested by the fire. +30 HP";
            blocked_t_ = 2.0f;
            return true;
        }

        default:
            // 资产编辑器里新加的可交互道具：没写专属逻辑，就给一句提示。
            if (props_.interactive(near_pid_ - 1)) {
                // 注意别写 blocked_hint_ = (...).c_str()：临时 string 当场就死了，
                // 指针悬空，屏幕上会显示一串乱码。要留一份在成员里。
                hint_buf_ = "You examine the " + props_.label(near_pid_ - 1) + ".";
                blocked_hint_ = hint_buf_.c_str();
                blocked_t_ = 2.0f;
                return true;
            }
            return false;
    }
}

void Game::check_interactions(float dt) {
    if (blocked_t_ > 0.f) blocked_t_ -= dt;

    // 每帧扫一次附近的宝箱/路牌/篝火，结果给"按 E"和"显示提示"共用
    scan_nearby_decor();

    if (portal_cd_ > 0.f) { portal_cd_ -= dt; return; }

    const int tx = (int(player_.x) + 32) / world::TileMap::TS;
    const int ty = (int(player_.y) + 32) / world::TileMap::TS;

    // 传送门
    for (const auto& s : map_.spawns) {
        if (s.type != "portal") continue;
        if (tx != s.x || ty != s.y) continue;

        if (!can_use_portal(s, quest_)) {
            blocked_hint_ = portal_blocked_reason(s, quest_);
            blocked_t_ = 2.0f;
            continue;
        }
        load_map(s.target_map, s.tx, s.ty);
        return;
    }

    // 装饰交互优先：站在宝箱前按 E，不该被旁边的 NPC 抢走
    if (player_.want_interact && try_decor_interact()) {
        player_.want_interact = false;
        return;
    }

    // NPC
    if (player_.want_interact) {
        player_.want_interact = false;

        for (const auto& s : map_.spawns) {
            if (s.type.rfind("npc_", 0) != 0) continue;

            const float ddx = float(s.x * world::TileMap::TS) - player_.x;
            const float ddy = float(s.y * world::TileMap::TS) - player_.y;
            if (ddx * ddx + ddy * ddy > 72.f * 72.f) continue;

            const char* kind = s.type.c_str() + 4;

            if (std::strcmp(kind, "elder") == 0) {
                active_lines_ = kElderIntro;
                quest_.met_elder = true;
                quest_.heartseed = true;
            } else if (std::strcmp(kind, "smith") == 0) {
                active_lines_ = kSmithSword;
                quest_.has_sword = true;
                if (inv_.weapon_index == 0) inv_.weapon_index = 1;
            } else {
                active_lines_ = kMerchantShop;
                inv_.potions += 1;
            }

            dialog_.start(active_lines_);
            state_ = State::Dialog;
            return;
        }
    }
}

// ---------------------------------------------------------------
void Game::update(float dt) {
    time_ += dt;

    // 编辑器点了 Reload：从磁盘重读地图，玩家位置血量不变
    if (reload_map_) {
        reload_map_ = false;
        reload_map_live();
    }

    // 素材热重载在**游戏中也跑**：你在 Aseprite 里改完一棵树，
    // 存盘 0.4 秒后游戏里那棵树就变了，不用重开。
    bank_.poll(dt);

    // ImGui 编辑器打开 = 世界暂停（但相机还能挪）
    if (editor_.active()) return;

    // 编辑模式：世界整个冻住（敌人不动、不挨打），
    // 只有相机和鼠标在工作。这样你可以安心改地图，不会被怪追着打。
    if (ed_.active) {
        editor_update(dt);
        return;
    }

    switch (state_) {
        case State::Playing:
            update_player(dt);
            update_enemies(dt);
            update_projectiles(dt);
            update_pickups(dt);
            update_fx(dt);
            check_interactions(dt);
            if (player_.hp <= 0) {
                player_.hp = 0;
                state_ = State::GameOver;
                gameover_t_ = 0.f;
            }
            break;

        case State::Dialog:
            update_dialog(dt);
            update_fx(dt);
            break;

        case State::GameOver:
            gameover_t_ += dt;
            break;

        case State::Ending:
            ending_t_ += dt;
            break;
    }

    update_weather(dt);
    cam_.update(dt);
    cam_.follow(int(player_.x), int(player_.y), fb_.width(), fb_.height(),
                map_.pixel_width(), map_.pixel_height());
}

// ---------------------------------------------------------------
void Game::draw_parallax() {
    if (!bg_loaded_) return;

    const float factor = 0.35f;
    const int bw = tex_bg_.width();
    if (bw <= 0) return;

    int offset = int(float(cam_.render_x()) * factor) % bw;
    if (offset < 0) offset += bw;

    for (int x = -offset; x < fb_.width(); x += bw) {
        gfx::blit_opaque(fb_, tex_bg_, {0, 0, bw, tex_bg_.height()}, x, 0);
    }
}

void Game::draw_world() {
    const int ts = world::TileMap::TS;
    const int cx = cam_.render_x();
    const int cy = cam_.render_y();

    const int x0 = cx / ts;
    const int y0 = cy / ts;
    const int x1 = (cx + fb_.width())  / ts + 1;
    const int y1 = (cy + fb_.height()) / ts + 1;

    for (int ty = y0; ty <= y1 && ty < map_.h; ++ty) {
        if (ty < 0) continue;
        for (int tx = x0; tx <= x1 && tx < map_.w; ++tx) {
            if (tx < 0) continue;
            const int idx = map_.at(tx, ty);
            gfx::blit_opaque(fb_, map_.image, map_.tile_rect(idx),
                             tx * ts - cx, ty * ts - cy);
        }
    }
}

void Game::draw_entities() {
    struct DrawCmd {
        const gfx::Surface* sheet;
        core::Rect src;
        core::Rect dst;
        int  sort_y;
        bool flash;
    };

    std::vector<DrawCmd> cmds;
    cmds.reserve(256);

    const int cx = cam_.render_x();
    const int cy = cam_.render_y();
    const int ts = world::TileMap::TS;

    // ---------------------------------------------------------------
    // 装饰物（树、木桶、墓碑……）
    //
    // 可见格要**向上多扫 6 格**：一棵 187px 的树从锚点往上长 5 格多，
    // 只扫可见格的话，站在树下面时树冠会被整个裁掉。
    // ---------------------------------------------------------------
    const int dx0 = std::max(0, cx / ts - 1);
    const int dy0 = std::max(0, cy / ts - 6);
    const int dx1 = std::min(map_.w - 1, (cx + fb_.width())  / ts + 1);
    const int dy1 = std::min(map_.h - 1, (cy + fb_.height()) / ts + 1);

    for (int ty = dy0; ty <= dy1; ++ty) {
        for (int tx = dx0; tx <= dx1; ++tx) {
            const int d = map_.decor_at(tx, ty);
            if (d == 0) continue;
            const int pi = d - 1;                 // 表里是 0-based，地图里是 1-based
            if (pi < 0 || pi >= props_.count()) continue;

            DrawCmd c;
            c.sheet  = &props_.image(pi);
            c.src    = { props_.sx(pi), props_.sy(pi), props_.sw(pi), props_.sh(pi) };
            c.dst    = { tx * ts + props_.ax(pi) - cx,
                         ty * ts + props_.ay(pi) - cy,
                         props_.dw(pi), props_.dh(pi) };
            c.sort_y = ty * ts + ts;              // 脚底 = 格子底部
            c.flash  = false;

            if (props_.tall(pi)) {
                cmds.push_back(c);                // 和角色一起排序，能挡住人
            } else {
                // 贴地类（树枝、小石头）不进排序：永远垫在实体下面
                gfx::blit_alpha(fb_, *c.sheet, c.src, c.dst.x, c.dst.y);
            }
        }
    }

    // 注意只要求 Transform + Sprite —— NPC 没有 Health，也要画出来
    reg_.each<Transform, Sprite>(
        [&](ecs::Entity e, Transform& t, Sprite& sp) {
            if (!sp.sheet) return;
            const Health* h = reg_.get<Health>(e);      // 可能为空
            DrawCmd c;
            c.sheet  = sp.sheet;
            c.src    = sp.anim.rect();
            c.dst    = { int(t.x) - cx, int(t.y) - cy, sp.draw_w, sp.draw_h };
            c.sort_y = int(t.y) + sp.draw_h;
            c.flash  = (h && h->flash > 0.f);
            cmds.push_back(c);
        });

    {
        DrawCmd c;
        c.sheet  = &tex_hero_walk_;
        c.src    = player_.anim.rect();
        c.dst    = { int(player_.x) - cx, int(player_.y) - cy, 64, 64 };
        c.sort_y = int(player_.y) + 64;
        c.flash  = (player_.invuln > 0.f && (int(time_ * 20.f) % 2 == 0));
        cmds.push_back(c);
    }

    std::sort(cmds.begin(), cmds.end(),
              [](const DrawCmd& a, const DrawCmd& b) { return a.sort_y < b.sort_y; });

    for (const auto& c : cmds) {
        // 同尺寸就别绕 blit_stretch —— 那个函数对每个像素都调
        // blend_pixel_scalar，一屏几十棵树会白烧掉一半帧时间。
        // blit_alpha 内部走 SIMD 的整行混合，快 3~5 倍。
        if (c.src.w == c.dst.w && c.src.h == c.dst.h) {
            gfx::blit_alpha(fb_, *c.sheet, c.src, c.dst.x, c.dst.y);
        } else {
            gfx::blit_stretch(fb_, *c.sheet, c.src, c.dst);
        }
        if (c.flash) gfx::fill_rect(fb_, c.dst, 0x50FF4040u);
    }
}

void Game::draw_projectiles() {
    const int cx = cam_.render_x();
    const int cy = cam_.render_y();

    reg_.each<Transform, Projectile>([&](ecs::Entity, Transform& t, Projectile& p) {
        const core::Pixel color = p.from_player ? 0xFFFFE080u : 0xFFFF5050u;
        gfx::fill_rect(fb_, { int(t.x) - cx - 3, int(t.y) - cy - 2, 7, 5 }, color);
    });
}

void Game::draw_pickups() {
    if (g_items.empty()) return;

    const int cx = cam_.render_x();
    const int cy = cam_.render_y();
    const int bob = int(std::sin(time_ * 4.f) * 2.f);

    reg_.each<Transform, Pickup>([&](ecs::Entity, Transform& t, Pickup& pk) {
        const size_t idx = size_t(pk.item_index) % g_items.size();
        const gfx::Surface& img = g_items[idx];
        const core::Rect src{ 0, 0, img.width(), img.height() };
        const core::Rect dst{ int(t.x) - cx, int(t.y) - cy + bob, 20, 20 };
        gfx::blit_stretch(fb_, img, src, dst);
    });
}

void Game::draw_fx() {
    if (g_fx.empty()) return;

    const int cx = cam_.render_x();
    const int cy = cam_.render_y();

    reg_.each<Transform, Fx>([&](ecs::Entity, Transform& t, Fx& f) {
        const float k = 1.f - f.life / 0.35f;          // 0 → 1
        const int size = int(24 + 24 * k);
        const gfx::Surface& img = g_fx[size_t(f.frame) % g_fx.size()];
        const core::Rect src{ 0, 0, img.width(), img.height() };
        const core::Rect dst{ int(t.x) - cx - size / 2, int(t.y) - cy - size / 2,
                              size, size };
        gfx::blit_stretch(fb_, img, src, dst);
    });
}

void Game::draw_weather() {
    const world::WeatherKind wk = world::from_string(map_.weather.c_str());

    // ---------------------------------------------------------------
    // 先铺一层"氛围色"。
    //
    // 只画雨丝/雪花是不够的 —— 在 640×360 上那就是几根细线，
    // 玩家根本意识不到"在下雨"。整屏压一层色，氛围立刻出来：
    //   雨 = 压暗 + 偏蓝，雪 = 冷白，流星 = 微红
    // ---------------------------------------------------------------
    if (wk == world::WeatherKind::Rain) {
        gfx::fill_rect_alpha(fb_, { 0, 0, fb_.width(), fb_.height() }, 0x28001028u);
    } else if (wk == world::WeatherKind::Snow) {
        gfx::fill_rect_alpha(fb_, { 0, 0, fb_.width(), fb_.height() }, 0x1CB0D0F0u);
    } else if (wk == world::WeatherKind::Meteor) {
        gfx::fill_rect_alpha(fb_, { 0, 0, fb_.width(), fb_.height() }, 0x20FF2800u);
    }

    if (wk == world::WeatherKind::Fog) {
        // 不能用 fill_rect：它是直接覆盖，整屏会被涂成一块灰板。
        // 雾是"叠一层"，必须逐像素 alpha 混合。
        const core::Pixel fog = 0x38C0C8D0u;
        for (int y = 0; y < fb_.height(); ++y) {
            core::Pixel* r = fb_.row(y);
            for (int x = 0; x < fb_.width(); ++x)
                gfx::blend_pixel_scalar(r[x], fog);
        }
        return;
    }
    if (wk == world::WeatherKind::None || particles_.count <= 0) return;

    const bool snow = (wk == world::WeatherKind::Snow);
    const core::Pixel color = snow ? 0xE0FFFFFFu : 0xA0B0C8D8u;

    for (int i = 0; i < particles_.count; ++i) {
        const int x = int(particles_.x[i]);
        const int y = int(particles_.y[i]);
        if (x < 0 || x >= fb_.width() || y < 0 || y >= fb_.height()) continue;

        // 雨线拉长到 12px：6px 在那个分辨率下几乎看不见
        const int len = snow ? 3 : 12;
        for (int k = 0; k < len && y + k < fb_.height(); ++k)
            gfx::blend_pixel_scalar(fb_.row(y + k)[x], color);
    }
}

void Game::draw_ui() {
    char buf[128];

    // Boss 进入第三阶段 → 全屏泛红（在 UI 之前画，血条才不被压暗）
    bool phase3 = false;
    reg_.each<Boss>([&](ecs::Entity, Boss& b) { if (b.phase == 3) phase3 = true; });
    if (phase3) {
        const uint32_t a = uint32_t(20 + 10 * std::sin(time_ * 4.f));
        gfx::fill_rect(fb_, { 0, 0, fb_.width(), fb_.height() },
                       (a << 24) | 0x200000u);
    }

    // 血量
    if (ui_style_ == UiStyle::Medieval) {
        gfx::draw_hearts(fb_, 8, 8, player_.hp, player_.max_hp,
                         tex_heart_full_, tex_heart_empty_);
    } else {
        gfx::draw_bar(fb_, 8, 8, 200, 16,
                      float(player_.hp) / float(player_.max_hp),
                      tex_bar_start_, tex_bar_mid_, tex_bar_end_);
    }

    // 武器 / 弹药
    const Weapon& w = WEAPONS[inv_.weapon_index];
    if (w.ammo >= 0) {
        std::snprintf(buf, sizeof(buf), "%s %d", w.name, inv_.ammo);
    } else {
        std::snprintf(buf, sizeof(buf), "%s", w.name);
    }
    gfx::draw_text(fb_, font_, 8, 28, buf, 0xFFE0E0E0u);

    // ---- 交互提示 ----
    // 不显示这个，玩家永远不知道宝箱能开、路牌能读。
    if (near_pid_ != 0 && state_ == State::Playing) {
        const char* what = nullptr;
        switch (near_pid_) {
            case world::prop_id::Chest: what = "[E] Open chest"; break;
            case world::prop_id::Sign1:
            case world::prop_id::Sign2: what = "[E] Read";       break;
            case world::prop_id::Fire:  what = "[E] Rest";       break;
            default:
                if (props_.interactive(near_pid_ - 1)) what = "[E] Inspect";
                break;
        }
        if (what) {
            const int w = int(std::strlen(what)) * 8;
            gfx::fill_rect_alpha(fb_, { fb_.width() / 2 - w / 2 - 4,
                                        fb_.height() - 46, w + 8, 14 }, 0xA0000000u);
            gfx::draw_text(fb_, font_, fb_.width() / 2 - w / 2,
                           fb_.height() - 44, what, 0xFFFFE060u);
        }
    }

    std::snprintf(buf, sizeof(buf), "GOLD %d", inv_.gold);
    gfx::draw_text(fb_, font_, fb_.width() - 8 * 10, 8, buf, 0xFFFFD060u);

    // 地图名
    gfx::draw_text(fb_, font_, 8, fb_.height() - 14,
                   current_map_.c_str(), 0xFF808890u);

    // 被门挡住的提示
    if (blocked_t_ > 0.f && blocked_hint_) {
        gfx::fill_rect(fb_, { 40, fb_.height() / 2 - 20, 560, 30 }, 0xC0000000u);
        gfx::draw_text(fb_, font_, 56, fb_.height() / 2 - 10,
                       blocked_hint_, 0xFFFFC060u);
    }

    // 对话
    if (state_ == State::Dialog && dialog_.active) {
        const core::Rect panel{ 16, fb_.height() - 116, fb_.width() - 32, 100 };
        gfx::draw_panel(fb_, panel, tex_ui_panel_, 8);

        const Line& ln = dialog_.current();
        gfx::draw_text(fb_, font_, panel.x + 10, panel.y + 10,
                       ln.who, 0xFFFFD080u);

        char text[256];
        int n = int(dialog_.chars_shown);
        if (n > 255) n = 255;
        std::memcpy(text, ln.text, size_t(n));
        text[n] = '\0';
        gfx::draw_text(fb_, font_, panel.x + 10, panel.y + 30, text, 0xFFFFFFFFu);

        gfx::draw_text(fb_, font_, panel.x + panel.w - 120, panel.y + panel.h - 16,
                       "E: NEXT", 0xFF808080u);
    }

    // 死亡
    if (state_ == State::GameOver) {
        gfx::fill_rect(fb_, { 0, 0, fb_.width(), fb_.height() }, 0xA0000000u);
        gfx::draw_text(fb_, font_, 270, 150, "YOU DIED", 0xFFFF4040u);
        gfx::draw_text(fb_, font_, 210, 180, "PRESS ESC TO QUIT", 0xFFC0C0C0u);
    }

    // 结局
    if (state_ == State::Ending) {
        gfx::fill_rect(fb_, { 0, 0, fb_.width(), fb_.height() }, 0xFF000000u);
        gfx::draw_text(fb_, font_, 220, 150, "ELDRA IS SEALED", 0xFFFFE0A0u);
        gfx::draw_text(fb_, font_, 190, 180, "BUT THE ROOTS REMEMBER", 0xFF808080u);
    }
}

void Game::render() {
    fb_.clear(0xFF101018u);

    // ImGui 编辑器：先把世界画出来当背景，再叠 UI
    if (editor_.active()) {
        draw_parallax();
        draw_world();
        draw_entities();
        draw_projectiles();
        draw_pickups();
        draw_fx();
        draw_weather();
        editor_.draw(&reload_map_);
        return;
    }

    if (ed_.active) {
        editor_draw();
        return;
    }

    draw_parallax();
    draw_world();
    draw_pickups();
    draw_entities();
    draw_projectiles();
    draw_fx();
    draw_weather();
    draw_ui();
}

} // namespace game
