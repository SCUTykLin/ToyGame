#include "platform/TextureCache.h"

#include <SDL.h>

#include <cstdio>

namespace platform {

TextureCache::~TextureCache() {
    // 这里**不能**调 shutdown() 的完整逻辑：
    // 析构时 renderer 可能已经没了，SDL_DestroyTexture 会崩。
    // 正常流程是 Game 在关窗口前显式调 shutdown()；
    // 这里只兜底把裸指针清掉，避免二次释放。
    map_.clear();
}

void TextureCache::shutdown() {
    for (auto& kv : map_) {
        if (kv.second.tex) SDL_DestroyTexture(kv.second.tex);
    }
    map_.clear();
}

void* TextureCache::get(const gfx::Surface& s, const std::string& key, uint64_t stamp) {
    if (!ren_ || s.width() <= 0 || s.height() <= 0) return nullptr;

    auto it = map_.find(key);
    if (it != map_.end()) {
        Entry& e = it->second;
        if (e.stamp == stamp && e.w == s.width() && e.h == s.height()) {
            return e.tex;                     // 没变，直接复用
        }
        // 变了：先销毁旧的，再建新的。顺序反了就是显存泄漏。
        if (e.tex) SDL_DestroyTexture(e.tex);
        map_.erase(it);
    }

    SDL_Texture* tex = SDL_CreateTexture(
        ren_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
        s.width(), s.height());
    if (!tex) {
        std::fprintf(stderr, "[tex] 创建纹理失败: %s\n", SDL_GetError());
        return nullptr;
    }
    SDL_SetTextureScaleMode(tex, SDL_ScaleModeNearest);   // 像素画不能糊
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    // pitch 是像素步长，SDL 要字节步长
    SDL_UpdateTexture(tex, nullptr, s.data(),
                      s.pitch() * int(sizeof(gfx::Pixel)));

    Entry e;
    e.tex = tex;
    e.stamp = stamp;
    e.w = s.width();
    e.h = s.height();
    map_[key] = e;
    return tex;
}

void TextureCache::clear() {
    shutdown();
}

} // namespace platform
