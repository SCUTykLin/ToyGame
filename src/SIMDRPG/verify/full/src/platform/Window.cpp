#include "platform/Window.h"
#include "platform/Input.h"      // 滚轮事件要转交给它

#include <SDL.h>
#include <cstdio>
#include <cstdlib>

namespace platform {

struct Window::Impl {
    SDL_Window*   win = nullptr;
    SDL_Renderer* ren = nullptr;
    SDL_Texture*  tex = nullptr;
};

Window::Window(int win_w, int win_h, int internal_w, int internal_h,
               const char* title)
    : iw_(internal_w), ih_(internal_h), ww_(win_w), wh_(win_h)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init 失败: %s\n", SDL_GetError());
        std::abort();
    }

    impl_ = new Impl();

    impl_->win = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        win_w, win_h,
        SDL_WINDOW_SHOWN);
    if (!impl_->win) {
        std::fprintf(stderr, "SDL_CreateWindow 失败: %s\n", SDL_GetError());
        std::abort();
    }

    impl_->ren = SDL_CreateRenderer(
        impl_->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // 没有显卡加速时（远程桌面、虚拟机、SDL_VIDEODRIVER=dummy）退化到软件渲染。
    // 不写这个 fallback 的话，无头自检会直接 abort。
    if (!impl_->ren) {
        impl_->ren = SDL_CreateRenderer(impl_->win, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!impl_->ren) {
        std::fprintf(stderr, "SDL_CreateRenderer 失败: %s\n", SDL_GetError());
        std::abort();
    }

    // 关键：纹理用内部分辨率，不是窗口分辨率
    impl_->tex = SDL_CreateTexture(
        impl_->ren,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        iw_, ih_);
    if (!impl_->tex) {
        std::fprintf(stderr, "SDL_CreateTexture 失败: %s\n", SDL_GetError());
        std::abort();
    }

    // 关键：最近邻放大，保持像素锐利。默认是线性插值，画面会糊。
    SDL_SetTextureScaleMode(impl_->tex, SDL_ScaleModeNearest);

    // 这里**故意不调用** SDL_RenderSetLogicalSize。
    //
    // 老版本靠它把 640×360 的画面自动铺满窗口，代价是所有绘制坐标都被
    // 隐式乘了 2。ImGui 的后端会读 SDL_RenderGetScale()，一旦发现不是 1
    // 就改用 FramebufferScale，两套缩放叠在一起，UI 会整体偏到屏幕外。
    // 现在改成"自己算目标矩形"：present 时把纹理铺到整个窗口，
    // 坐标系统保持 1:1（1280×720），ImGui 不用猜。
}

Window::~Window() {
    if (impl_) {
        if (impl_->tex) SDL_DestroyTexture(impl_->tex);
        if (impl_->ren) SDL_DestroyRenderer(impl_->ren);
        if (impl_->win) SDL_DestroyWindow(impl_->win);
        delete impl_;
        impl_ = nullptr;
    }
    SDL_Quit();
}

void Window::begin_scene(const uint32_t* pixels, int pitch_pixels) {
    // pitch 是**像素**步长，SDL 要**字节**步长，所以乘 4。
    // SDL_UpdateTexture 的 pitch 参数单位是**字节**
    SDL_UpdateTexture(impl_->tex, nullptr, pixels,
                      pitch_pixels * static_cast<int>(sizeof(uint32_t)));
    SDL_RenderClear(impl_->ren);

    // 铺满窗口。窗口固定 1280×720、画面 640×360，正好 2 倍。
    // 想加"保持宽高比"的黑边，也就改这四行。
    const SDL_Rect dst{ 0, 0, ww_, wh_ };
    SDL_RenderCopy(impl_->ren, impl_->tex, nullptr, &dst);
}

void Window::present() {
    SDL_RenderPresent(impl_->ren);
}

void Window::present(const uint32_t* pixels, int pitch_pixels) {
    begin_scene(pixels, pitch_pixels);
    present();
}

bool Window::read_pixels(uint32_t* dst, int dst_pitch_pixels) const {
    if (!impl_ || !impl_->ren) return false;
    return SDL_RenderReadPixels(impl_->ren, nullptr, SDL_PIXELFORMAT_ARGB8888,
                                dst, dst_pitch_pixels * int(sizeof(uint32_t))) == 0;
}

void* Window::native_window() const {
    return impl_ ? impl_->win : nullptr;
}

void* Window::native_renderer() const {
    return impl_ ? impl_->ren : nullptr;
}

bool Window::poll() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // UI 先看：ImGui 处理完会告诉我们"这条我用掉了"
        const bool consumed = filter_ ? filter_(&e) : false;

        if (e.type == SDL_QUIT) {
            running_ = false;
        } else if (e.type == SDL_KEYDOWN) {
            // ESC 是"退出游戏"的老快捷键。但用户正在 ImGui 输入框里
            // 按 ESC 取消输入时，这条事件已经被 UI 吃掉了，不能顺手退游戏。
            if (!consumed && e.key.keysym.sym == SDLK_ESCAPE) running_ = false;
        } else if (e.type == SDL_MOUSEWHEEL) {
            // 滚轮没有"查询式"API（SDL_GetMouseState 拿不到它），
            // 只能从事件里拿。所以在这里转交给 Input 累计。
            Input::add_wheel(e.wheel.y);
        }
    }
    return running_;
}

} // namespace platform
