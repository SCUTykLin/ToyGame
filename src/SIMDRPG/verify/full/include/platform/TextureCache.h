#pragma once
#include "gfx/Surface.h"

#include <cstdint>
#include <string>
#include <unordered_map>

struct SDL_Renderer;
struct SDL_Texture;

namespace platform {

// ---------------------------------------------------------------
// Surface → SDL_Texture 的桥
//
// ImGui::Image() 要的是"显卡里的纹理句柄"，不是我们那块 CPU 内存。
// 所以每张要显示的图都得先传到 GPU 一次。
//
// 三个必须处理的点：
//   1. 别每帧上传。一张 1024×1024 的图每帧传一次，帧率直接腰斩。
//      → 用 key 缓存，key 里带版本号（文件 mtime）。
//   2. 文件热重载后 key 变了，旧纹理要销毁，否则显存泄漏。
//   3. 析构顺序：纹理属于 renderer，renderer 属于 Window。
//      cache 必须比 Window 先死（Game 里成员声明顺序就是析构顺序的反序）。
// ---------------------------------------------------------------
class TextureCache {
public:
    ~TextureCache();

    void init(SDL_Renderer* ren) { ren_ = ren; }
    void shutdown();

    // stamp 变了就重建纹理。stamp 用文件的 mtime 最省事。
    //
    // 返回 void* 而不是 ImTextureID：这个头文件就不必 include imgui.h，
    // 用到的地方写 (ImTextureID)cache.get(...) 转一下就行
    //（1.89 的 ImTextureID 本来就是 void*，等于没转）。
    void* get(const gfx::Surface& s, const std::string& key, uint64_t stamp);

    void clear();
    int  count() const { return int(map_.size()); }

private:
    struct Entry {
        SDL_Texture* tex = nullptr;
        uint64_t     stamp = 0;
        int w = 0, h = 0;
    };

    SDL_Renderer* ren_ = nullptr;
    std::unordered_map<std::string, Entry> map_;
};

} // namespace platform
