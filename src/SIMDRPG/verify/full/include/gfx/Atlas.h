#pragma once
#include "gfx/Surface.h"
#include "core/Types.h"

#include <string>
#include <vector>

namespace gfx {

using core::Pixel;
using core::Rect;

// 读一张 PNG，统一转成 ARGB8888。
// 失败直接 abort —— 资源缺失是开发期错误，不该带着半个游戏继续跑。
Surface load_png(const std::string& path);

// 读进一块已经存在的 Surface；**失败返回 false，不 abort**。
//
// 为什么要有这个版本：热重载时你随时可能正把一张图存一半，
// 或者刚删掉还没来得及替换。这时候 abort 会把整个游戏带走，
// 而"这次读失败，保留上一帧的像素"才是正确行为。
// 另外它还能复用已经分配好的内存（尺寸没变的情况下）。
bool load_png_into(Surface& out, const std::string& path);

// 读一个目录下所有 .png（不递归）。用来一次性加载 440 个道具。
std::vector<Surface> load_dir(const std::string& dir);

// 一整张图 + 帧尺寸。取第 index 帧靠算，不预先切好。
struct Atlas {
    Surface image;
    int frame_w = 0;
    int frame_h = 0;

    Rect frame(int index) const {
        if (frame_w <= 0 || frame_h <= 0)
            return {0, 0, image.width(), image.height()};
        const int cols = image.width() / frame_w;
        return { (index % cols) * frame_w,
                 (index / cols) * frame_h,
                 frame_w, frame_h };
    }
};

// 角色序列帧：一行一个朝向，一列一帧。
// 行序固定为 0=背 1=左 2=正 3=右（LPC 约定，见阶段 7）
inline Rect sprite_frame(int dir, int f, int fw = 64, int fh = 64) {
    return { f * fw, dir * fh, fw, fh };
}

} // namespace gfx
