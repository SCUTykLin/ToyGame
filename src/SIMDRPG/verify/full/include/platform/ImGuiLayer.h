#pragma once

namespace platform {

class Window;

// ---------------------------------------------------------------
// ImGui 的一层薄封装
//
// 它只干四件事：
//   init()        建 context、接 SDL2 + SDLRenderer2 后端、装事件过滤器
//   begin_frame() 每帧开头：两个后端的 NewFrame + ImGui::NewFrame
//   end_frame()   ImGui::Render + 把 draw data 交给 SDL_Renderer
//   shutdown()    逆序拆掉
//
// 为什么不把这堆塞进 Game：
//   换后端（比如以后换 OpenGL）只改这一个文件，Game 一行不动。
// ---------------------------------------------------------------
class ImGuiLayer {
public:
    bool init(Window& win);
    void shutdown();

    void begin_frame();
    void end_frame();

    bool ready() const { return ready_; }

    // ImGui 想不想要鼠标/键盘。游戏输入要拿这个做门禁，
    // 否则你在输入框里打 "wasd"，角色会一边走一边打字。
    bool wants_mouse() const;
    bool wants_keyboard() const;

private:
    bool ready_ = false;
};

} // namespace platform
