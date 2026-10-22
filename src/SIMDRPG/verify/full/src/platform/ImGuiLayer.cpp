#include "platform/ImGuiLayer.h"
#include "platform/Window.h"

#include <SDL.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include <cstdio>

namespace platform {

namespace {

// 事件过滤器：Window::poll() 每收到一条事件就先喂给 ImGui。
//
// 返回 true 的含义是"这条事件被 UI 消费了"。
// 最典型的场景：你在搜索框里打字，敲了 "w"，游戏不该让角色往前走。
bool sdl_event_filter(const void* e) {
    const SDL_Event* ev = static_cast<const SDL_Event*>(e);
    return ImGui_ImplSDL2_ProcessEvent(ev);
}

} // namespace

bool ImGuiLayer::init(Window& win) {
    if (ready_) return true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;          // 不生成 imgui.ini，窗口布局全在代码里
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 默认字体只有 13px。窗口是 1280×720，13px 小得像蚂蚁；
    // AddFontDefault 会按 SizePixels 重新栅格化，18px 正好。
    ImFontConfig fc;
    fc.SizePixels  = 18.f;
    fc.OversampleH = 1;
    fc.OversampleV = 1;
    fc.PixelSnapH  = true;
    io.Fonts->AddFontDefault(&fc);

    // SDL2 后端负责键鼠/窗口事件，SDLRenderer2 后端负责把三角形画出来。
    // 顺序不能反：Renderer 后端初始化时会去建字体纹理。
    SDL_Window*   sdl_win = static_cast<SDL_Window*>(win.native_window());
    SDL_Renderer* sdl_ren = static_cast<SDL_Renderer*>(win.native_renderer());

    if (!ImGui_ImplSDL2_InitForSDLRenderer(sdl_win, sdl_ren)) {
        std::fprintf(stderr, "[imgui] SDL2 后端初始化失败\n");
        return false;
    }
    if (!ImGui_ImplSDLRenderer2_Init(sdl_ren)) {
        std::fprintf(stderr, "[imgui] SDLRenderer2 后端初始化失败\n");
        return false;
    }

    ImGuiStyle& st = ImGui::GetStyle();
    st.WindowRounding  = 4.f;
    st.FrameRounding   = 3.f;
    st.ScrollbarRounding = 3.f;
    st.WindowBorderSize = 1.f;

    win.set_event_filter(&sdl_event_filter);
    ready_ = true;
    std::printf("[imgui] %s 已就绪\n", IMGUI_VERSION);
    return true;
}

void ImGuiLayer::shutdown() {
    if (!ready_) return;
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    ready_ = false;
}

void ImGuiLayer::begin_frame() {
    if (!ready_) return;

    // 两个后端各有一个 NewFrame，缺一个 ImGui::NewFrame 就会断言失败
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end_frame() {
    if (!ready_) return;
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiLayer::wants_mouse() const {
    return ready_ && ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiLayer::wants_keyboard() const {
    return ready_ && ImGui::GetIO().WantCaptureKeyboard;
}

} // namespace platform
