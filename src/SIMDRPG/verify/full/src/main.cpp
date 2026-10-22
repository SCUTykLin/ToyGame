#include "game/Game.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>

namespace {

void use_dummy_video() {
#ifdef _WIN32
    _putenv_s("SDL_VIDEODRIVER", "dummy");   // 无头模式：不创建窗口
#else
    setenv("SDL_VIDEODRIVER", "dummy", 1);
#endif
}

} // namespace

int main(int argc, char** argv) {
    int  frames    = -1;         // -1 = 一直跑
    bool selftest  = false;

    // 截图模式：--shot <地图> <x> <y> <帧数> <输出.png>
    //           --edit-shot 同上，但进手搓编辑器（F1）再截
    //           --ui-shot   同上，但进 ImGui 编辑器（F2）再截
    const char* shot_map = nullptr;
    const char* shot_out = nullptr;
    int  shot_x = 32, shot_y = 20;
    bool edit_shot = false;
    bool ui_shot   = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--selftest") == 0) {
            selftest = true;
            frames = (i + 1 < argc) ? std::atoi(argv[i + 1]) : 120;
            use_dummy_video();
        } else if ((std::strcmp(argv[i], "--shot") == 0 ||
                    std::strcmp(argv[i], "--edit-shot") == 0 ||
                    std::strcmp(argv[i], "--ui-shot") == 0) && i + 5 < argc) {
            edit_shot = (std::strcmp(argv[i], "--edit-shot") == 0);
            ui_shot   = (std::strcmp(argv[i], "--ui-shot") == 0);
            shot_map  = argv[i + 1];
            shot_x    = std::atoi(argv[i + 2]);
            shot_y    = std::atoi(argv[i + 3]);
            frames    = std::atoi(argv[i + 4]);
            shot_out  = argv[i + 5];
            use_dummy_video();
            i += 5;
        }
    }

    try {
        game::Game g;
        if (shot_map) {
            g.warp(shot_map, shot_x, shot_y);   // 直接跳到目标地图
            if (edit_shot) g.enter_editor();    // --edit-shot：手搓编辑器
            if (ui_shot)   g.enter_studio();    // --ui-shot：ImGui 编辑器
        }

        // ImGui 画在窗口上，不在 fb_ 里，所以要读 backbuffer
        if (ui_shot && shot_out) g.request_capture(shot_out);

        g.run(frames);

        // ImGui 的截图已经在帧内完成了；普通截图才走这里
        if (shot_out && !ui_shot && !g.save_png(shot_out)) return 2;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "致命错误: %s\n", e.what());
        return 1;
    }
    (void)selftest;
    return 0;
}
