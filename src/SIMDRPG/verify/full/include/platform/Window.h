#pragma once
#include <cstdint>

namespace platform {

// 窗口 + 渲染器 + 一张流式纹理。
//
// 设计要点：头文件里**不出现 <SDL.h>**。
// 用一个不透明的 Impl 指针把 SDL 类型藏进 .cpp，
// 这样 gfx / game 层 include 这个头时，不会被迫也 include SDL。
class Window {
public:
    // win_w/win_h      : 窗口在屏幕上的大小
    // internal_w/h     : 你实际绘制的分辨率
    Window(int win_w, int win_h, int internal_w, int internal_h, const char* title);
    ~Window();

    // 管着 SDL 资源，拷贝会导致双重释放
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // ---------------------------------------------------------------
    // 一帧的绘制顺序（接了 ImGui 之后）
    //
    //   begin_scene(pixels, pitch)  上传游戏画面 + 清屏 + 画上去
    //   ... ImGui 在这里画 ...
    //   present()                   SDL_RenderPresent
    //
    // 拆成两半是因为 ImGui 必须画在"游戏画面之上、Present 之前"。
    // 老版只有一个 present()，把三步焊死了，加 UI 时只能整个改。
    // ---------------------------------------------------------------
    void begin_scene(const uint32_t* pixels, int pitch_pixels);
    void present();

    // 兼容旧写法：上传 + Present（不画 UI）。
    void present(const uint32_t* pixels, int pitch_pixels);

    // 把当前窗口的像素读回来（含 ImGui 画的内容）。
    // 必须在 present() 之前、画完之后调 —— present 之后 backbuffer 可能是空的。
    bool read_pixels(uint32_t* dst, int dst_pitch_pixels) const;

    // 处理事件。返回 false 表示该退出了（点了关闭或按了 ESC）
    bool poll();

    // ---------------------------------------------------------------
    // 给 UI 层（ImGui）用的三个口子
    //
    // 为什么返回 void* 而不是 SDL_Window*：这个头文件仍然不 include SDL。
    // 调用方（ImGuiLayer）自己 include <SDL.h> 再 static_cast 回去。
    // ---------------------------------------------------------------
    void* native_window()   const;
    void* native_renderer() const;

    // 事件过滤器：在游戏处理之前把每条事件给 UI 看一眼。
    // 返回 true = UI 吃掉了（比如正在输入框里打字），游戏别再处理。
    //
    // 参数是 const SDL_Event*，但这里只能写成 const void* ——
    // 头文件不认识 SDL_Event。
    using EventFilter = bool (*)(const void* sdl_event);
    void set_event_filter(EventFilter f) { filter_ = f; }

    int internal_width()  const { return iw_; }
    int internal_height() const { return ih_; }

    // 窗口在屏幕上的实际尺寸。
    // 鼠标事件报的是**窗口**坐标，而画面上的一格是**内部**坐标，
    // 两者差一个缩放比：窗口 1280×720 显示 640×360 的画面，差 2 倍。
    // 编辑器要靠这个换算，否则点哪儿画哪儿会偏移一倍。
    int window_width()  const { return ww_; }
    int window_height() const { return wh_; }

private:
    struct Impl;                 // 定义在 .cpp 里
    Impl* impl_ = nullptr;
    int iw_ = 0, ih_ = 0;
    int ww_ = 0, wh_ = 0;        // 窗口尺寸（屏幕像素）
    bool running_ = true;
    EventFilter filter_ = nullptr;
};

} // namespace platform
