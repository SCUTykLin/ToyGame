#pragma once
#include <cstdint>

namespace platform {

// 键盘 + 鼠标。全是 static —— 一个进程只有一个输入设备，
// 传对象到处跑没有意义。
class Input {
public:
    static void begin_frame();     // 循环开头调用
    static void end_frame();       // 循环结尾调用

    static bool down(int scancode);      // 按着
    static bool pressed(int scancode);   // 刚按下（只有这一帧为真）

    // ---------------------------------------------------------------
    // 鼠标
    //
    // 坐标是**窗口**像素，不是内部分辨率。
    // 窗口 1280×720 显示 640×360 的画面，鼠标坐标要除以 2 才能对上格子。
    // 换算不在这里做 —— Input 只报原始值，不猜你要拿它干什么。
    // （Game 里用 window_width() / internal_width() 算比例。）
    // ---------------------------------------------------------------
    static int mouse_x();
    static int mouse_y();

    static bool mouse_down(int button);      // 1=左 2=中 3=右
    static bool mouse_pressed(int button);   // 这一帧刚按下
    static bool mouse_released(int button);

    // 本帧滚轮增量：上滚 +1、下滚 -1。end_frame 清零。
    static int wheel();

    // 滚轮没有查询式 API，只能从事件里拿。
    // Window::poll 收到 SDL_MOUSEWHEEL 时调这里累加。
    static void add_wheel(int dy);

private:
    static const unsigned char* curr_;
    static unsigned char prev_[512];

    static int      mx_, my_;
    static uint32_t buttons_, buttons_prev_;
    static int      wheel_;
};

} // namespace platform
