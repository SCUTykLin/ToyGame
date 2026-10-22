#include "gfx/Atlas.h"

#include <SDL_image.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>

namespace gfx {

bool load_png_into(Surface& out, const std::string& path) {
    SDL_Surface* raw = IMG_Load(path.c_str());
    if (!raw) {
        // 热重载路径：读失败是"常态"（文件正被写、刚被删），不是致命错误
        std::fprintf(stderr, "IMG_Load 失败: %s (%s)\n", path.c_str(), IMG_GetError());
        return false;
    }

    // 统一转格式。这一步之后每像素一定是 4 字节、0xAARRGGBB。
    SDL_Surface* conv = SDL_ConvertSurfaceFormat(raw, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(raw);
    if (!conv) {
        std::fprintf(stderr, "格式转换失败: %s\n", path.c_str());
        return false;
    }

    Surface tmp(conv->w, conv->h);

    // 逐行拷贝。不能一次 memcpy 整张 —— 两边的 pitch 不一定相等。
    for (int y = 0; y < conv->h; ++y) {
        const uint8_t* src = static_cast<const uint8_t*>(conv->pixels)
                           + size_t(y) * size_t(conv->pitch);
        std::memcpy(tmp.row(y), src, size_t(conv->w) * sizeof(Pixel));
    }

    SDL_FreeSurface(conv);
    out = std::move(tmp);          // 移动赋值，老缓冲自动释放
    return true;
}

Surface load_png(const std::string& path) {
    Surface out;
    if (!load_png_into(out, path)) std::abort();
    return out;
}

std::vector<Surface> load_dir(const std::string& dir) {
    std::vector<Surface> out;
    namespace fs = std::filesystem;

    if (!fs::exists(dir)) {
        std::fprintf(stderr, "目录不存在: %s\n", dir.c_str());
        return out;
    }

    // 递归：objects/ 下面还有子目录（Ores & Ingots、food、weapons...），
    // 不递归的话只能拿到 238 个，递归才是完整的 440 个。
    for (const auto& e : fs::recursive_directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() != ".png") continue;
        out.push_back(load_png(e.path().string()));
    }
    return out;
}

} // namespace gfx
