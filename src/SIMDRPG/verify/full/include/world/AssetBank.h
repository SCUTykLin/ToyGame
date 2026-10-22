#pragma once
#include "gfx/Surface.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace world {

// ---------------------------------------------------------------
// 素材库里的一条记录
//
// 注意 w/h 是"解码之后才知道"的：扫描阶段只走目录、不开文件。
// 1195 张图全解码要 353MB，开机全读进来是自杀。
// ---------------------------------------------------------------
struct AssetInfo {
    std::string rel;     // 相对 root 的路径，统一用 '/'：backgrounds/tree-1.png
    std::string name;    // 文件名：tree-1.png
    std::string dir;     // 顶层目录：backgrounds；根目录下的图是空串
    int w = 0, h = 0;    // 原始像素尺寸（0 = 还没解码过）
};

// ---------------------------------------------------------------
// 素材库：整个 assets/gfx 的目录索引 + 按需解码 + 磁盘变化检测
//
// 三个职责，彼此独立：
//   1. scan()   —— 只读目录，建索引。几百毫秒都不到。
//   2. image()  —— 第一次要才解码，之后常驻（并可被 pin 住）。
//      thumb()   —— 只在内存里留一张 ≤44px 的缩略图给浏览器用。
//   3. poll()   —— 每 0.4 秒看一眼已加载文件的 mtime/size，
//                 变了就重新解码 —— 这就是"热重载"。
//
// 为什么要有预算：353MB 全解码放不下（还要留给 framebuffer 和贴图），
// 所以全尺寸图有 64MB 上限，超了按 LRU 卸掉；被 pin 的（正在当道具用的）
// 永远不卸。
// ---------------------------------------------------------------
class AssetBank {
public:
    AssetBank() = default;
    explicit AssetBank(std::string root) : root_(std::move(root)) {}

    void set_root(std::string root) { root_ = std::move(root); }
    const std::string& root() const { return root_; }

    // 重扫目录。会清空所有已解码的图（所以调用方要重 pin）。
    // 返回扫到的文件数。
    int scan();

    int  count() const { return int(items_.size()); }
    bool valid(int i) const { return i >= 0 && i < count(); }
    const AssetInfo& info(int i) const;

    // rel 用 '/' 分隔。找不到返回 -1。
    int index_of(const std::string& rel) const;

    // 全尺寸图。第一次调用会解码，并把这条"钉住"（trim 不会卸它）。
    // 读失败返回一张 2×2 的洋红占位图，不会崩、不会 abort。
    const gfx::Surface& image(int i);

    // 缩略图（最长边 kThumbMax）。生成完如果没被 pin，会把全尺寸图放掉。
    const gfx::Surface& thumb(int i);

    void pin(int i)   { if (valid(i)) items_[size_t(i)].pinned = true; }
    void unpin(int i) { if (valid(i)) items_[size_t(i)].pinned = false; }
    bool pinned(int i) const { return valid(i) && items_[size_t(i)].pinned; }

    // 按预算卸掉最久没用的全尺寸图。别在持有 image() 引用时调用。
    void trim();

    // 立刻检查磁盘，返回这次重载了几个文件。
    int  check_disk();
    // 累计 dt，每 0.4 秒做一次 check_disk。游戏主循环里每帧调。
    void poll(float dt);
    // 强制重载一张（不看 mtime）。用于 F10。
    bool reload(int i);

    int    reload_count() const { return reloads_; }
    int    loaded_count() const;
    size_t memory_bytes() const;
    size_t budget_bytes() const { return budget_; }

    // 版本号（mtime + 文件大小）。给 TextureCache 当缓存的 key 用：
    // 文件一变这个数就变，纹理会自动重建。
    uint64_t stamp(int i) const;

private:
    struct Item {
        AssetInfo info;
        std::unique_ptr<gfx::Surface> full;
        std::unique_ptr<gfx::Surface> thumb;
        int64_t  mtime = 0;
        int64_t  size  = 0;
        uint32_t used  = 0;      // 最近一次访问的 tick，LRU 用
        bool     pinned = false;
        bool     bad = false;    // 上次解码失败；check_disk 会重试
    };

    bool load_full(int i);
    void touch(int i) { items_[size_t(i)].used = ++tick_; }

    std::string root_;
    std::vector<Item> items_;
    std::unordered_map<std::string, int> by_rel_;

    uint32_t tick_      = 0;
    float    poll_acc_  = 0.f;
    int      reloads_   = 0;
    size_t   budget_    = 64ull * 1024 * 1024;
};

} // namespace world
