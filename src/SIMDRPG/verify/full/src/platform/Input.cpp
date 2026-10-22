#include "platform/Input.h"

#include <SDL.h>
#include <cstring>

namespace platform {

const unsigned char* Input::curr_ = nullptr;
unsigned char Input::prev_[512] = {};

int      Input::mx_ = 0;
int      Input::my_ = 0;
uint32_t Input::buttons_ = 0;
uint32_t Input::buttons_prev_ = 0;
int      Input::wheel_ = 0;

void Input::begin_frame() {
    curr_ = SDL_GetKeyboardState(nullptr);

    // 鼠标位置和按键是查询式的，不用处理事件
    int x = 0, y = 0;
    buttons_ = SDL_GetMouseState(&x, &y);
    mx_ = x;
    my_ = y;

    // wheel_ 故意不在这里清零。
    // 它是在 Window::poll() 里累加的，而 poll() 在 begin_frame() **之前**跑完。
    // 在这里清会把这一帧刚滚的那几下抹掉。
}

void Input::end_frame() {
    if (curr_) std::memcpy(prev_, curr_, SDL_NUM_SCANCODES);
    buttons_prev_ = buttons_;
    wheel_ = 0;                  // 本帧读完了，清掉
}

bool Input::down(int sc) {
    if (!curr_ || sc < 0 || sc >= SDL_NUM_SCANCODES) return false;
    return curr_[sc] != 0;
}

bool Input::pressed(int sc) {
    if (!curr_ || sc < 0 || sc >= SDL_NUM_SCANCODES) return false;
    const bool now = curr_[sc] != 0;
    const bool was = prev_[sc] != 0;
    return now && !was;
}

int Input::mouse_x() { return mx_; }
int Input::mouse_y() { return my_; }

bool Input::mouse_down(int b) {
    if (b < 1 || b > 3) return false;
    return (buttons_ & SDL_BUTTON(b)) != 0;
}

bool Input::mouse_pressed(int b) {
    if (b < 1 || b > 3) return false;
    const uint32_t mask = SDL_BUTTON(b);
    return (buttons_ & mask) != 0 && (buttons_prev_ & mask) == 0;
}

bool Input::mouse_released(int b) {
    if (b < 1 || b > 3) return false;
    const uint32_t mask = SDL_BUTTON(b);
    return (buttons_ & mask) == 0 && (buttons_prev_ & mask) != 0;
}

int  Input::wheel() { return wheel_; }
void Input::add_wheel(int dy) { wheel_ += dy; }

} // namespace platform
