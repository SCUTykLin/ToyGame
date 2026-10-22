#include "world/AssetBank.h"
#include "gfx/Atlas.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <system_error>

namespace world {

namespace fs = std::filesystem;

namespace {

// 缩略图最长边。格子是 40px，44 已经够锐利了；再大纯属浪费内存。
constexpr int kThumbMax = 44;

bool is_image(const fs::path& p) {
    const std::string ext = p.extension().string();
    return ext == ".png" || ext == ".PNG" ||
           ext == ".jpg" || ext == ".jpeg";
}

// mtime 在有些文件系统上只有 1 秒精度，所以同时比文件大小 ——
// 只靠 mtime 会漏掉"同一秒内改完的图"，改完看不见变化最让人怀疑人生。
int64_t mtime_of(const fs::path& p, int64_t* size) {
    std::error_code ec;
    const auto t = fs::last_write_time(p, ec);
    if (ec) return 0;
    if (size) {
        std::error_code ec2;
        const auto s = fs::file_size(p, ec2);
        *size = ec2 ? 0 : int64_t(s);
    }
    return int64_t(t.time_since_epoch().count());
}

const gfx::Surface& placeholder() {
    static const gfx::Surface p = [] {
        gfx::Surface s(2, 2);
        s.clear(0xFFFF00FFu);      // 洋红：图形学里约定俗成的"这里缺东西"
        return s;
    }();
    return p;
}

} // namespace

const AssetInfo& AssetBank::info(int i) const {
    static const AssetInfo empty{};
    if (!valid(i)) return empty;
    return items_[size_t(i)].info;
}

int AssetBank::scan() {
    items_.clear();
    by_rel_.clear();

    std::error_code ec;
    if (!fs::is_directory(root_, ec)) {
        std::fprintf(stderr, "[assets] 目录不存在: %s\n", root_.c_str());
        return 0;
    }

    int skipped = 0;
    for (auto it = fs::recursive_directory_iterator(
             root_, fs::directory_options::skip_permission_denied, ec);
         it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (ec) { ec.clear(); continue; }          // 某个子目录读不动，跳过

        const fs::directory_entry& e = *it;
        if (!e.is_regular_file(ec) || ec) { ec.clear(); continue; }

        const fs::path& p = e.path();
        if (!is_image(p)) continue;

        // 下划线开头的是我们自己生成的拼图（_contact_*.png），
        // 768×3360 那种，既不该出现在浏览器里也不该被当成素材。
        const std::string name = p.filename().string();
        if (!name.empty() && (name[0] == '_' || name[0] == '.')) { ++skipped; continue; }

        std::error_code ec2;
        std::string rel = fs::relative(p, root_, ec2).generic_string();
        if (ec2 || rel.empty()) { ec2.clear(); continue; }

        Item item;
        item.info.rel  = rel;
        item.info.name = name;

        const size_t slash = rel.find('/');
        item.info.dir = (slash == std::string::npos) ? "" : rel.substr(0, slash);

        item.mtime = mtime_of(p, &item.size);
        items_.push_back(std::move(item));
    }

    // 按路径排序：两次 scan 的顺序必须一致，否则"第 500 个素材"
    // 这种下标（以及所有截图、文档里的编号）会漂。
    std::sort(items_.begin(), items_.end(), [](const Item& a, const Item& b) {
        return a.info.rel < b.info.rel;
    });

    for (int i = 0; i < count(); ++i)
        by_rel_[items_[size_t(i)].info.rel] = i;

    std::printf("[assets] 扫描 %s：%d 个文件（跳过 %d 个 _ 开头）\n",
                root_.c_str(), count(), skipped);
    return count();
}

int AssetBank::index_of(const std::string& rel) const {
    const auto it = by_rel_.find(rel);
    return (it == by_rel_.end()) ? -1 : it->second;
}

bool AssetBank::load_full(int i) {
    Item& it = items_[size_t(i)];
    if (it.full) return true;
    if (it.bad)  return false;          // 失败过就不再每帧重试，等 check_disk

    const std::string path = root_ + "/" + it.info.rel;
    gfx::Surface img;
    if (!gfx::load_png_into(img, path)) {
        it.bad = true;
        std::fprintf(stderr, "[assets] 读不了 %s（先给占位图）\n", path.c_str());
        return false;
    }

    it.bad  = false;
    it.info.w = img.width();
    it.info.h = img.height();
    it.full = std::make_unique<gfx::Surface>(std::move(img));
    return true;
}

const gfx::Surface& AssetBank::image(int i) {
    if (!valid(i)) return placeholder();

    Item& it = items_[size_t(i)];
    if (!it.full && !load_full(i)) return placeholder();

    touch(i);
    it.pinned = true;                  // 当道具用的图不允许被 trim 掉
    return *it.full;
}

const gfx::Surface& AssetBank::thumb(int i) {
    if (!valid(i)) return placeholder();

    Item& it = items_[size_t(i)];
    if (it.thumb) return *it.thumb;
    if (!it.full && !load_full(i)) return placeholder();

    const gfx::Surface& src = *it.full;
    const int sw = src.width();
    const int sh = src.height();
    if (sw <= 0 || sh <= 0) return placeholder();

    // 等比缩放，最长边不超过 kThumbMax
    int tw, th;
    if (sw >= sh) { tw = kThumbMax; th = std::max(1, sh * kThumbMax / sw); }
    else          { th = kThumbMax; tw = std::max(1, sw * kThumbMax / sh); }

    auto t = std::make_unique<gfx::Surface>(tw, th);
    for (int y = 0; y < th; ++y) {
        for (int x = 0; x < tw; ++x) {
            // 最近邻取样：缩略图只要"看得出是什么"，不要双线性那种糊
            const int sx = x * sw / tw;
            const int sy = y * sh / th;
            t->row(y)[x] = src.row(sy)[sx];
        }
    }

    it.thumb = std::move(t);
    touch(i);

    // 缩略图已经拿到了，全尺寸图如果没人钉着就放掉 ——
    // 这是"浏览 1195 张图只花十几 MB"的关键。
    if (!it.pinned) it.full.reset();
    return *it.thumb;
}

void AssetBank::trim() {
    while (memory_bytes() > budget_) {
        int victim = -1;
        uint32_t oldest = UINT32_MAX;
        for (int i = 0; i < count(); ++i) {
            const Item& it = items_[size_t(i)];
            if (!it.full || it.pinned) continue;
            if (it.used < oldest) { oldest = it.used; victim = i; }
        }
        if (victim < 0) break;         // 全是 pin 的，只能认了
        items_[size_t(victim)].full.reset();
    }
}

bool AssetBank::reload(int i) {
    if (!valid(i)) return false;

    Item& it = items_[size_t(i)];
    const std::string path = root_ + "/" + it.info.rel;

    int64_t sz = 0;
    const int64_t mt = mtime_of(path, &sz);

    gfx::Surface img;
    if (!gfx::load_png_into(img, path)) {
        // 读失败：保留旧像素。宁可看到旧图，也不要看到一片黑。
        it.bad = true;
        std::fprintf(stderr, "[assets] 重载失败，保留旧图: %s\n", path.c_str());
        return false;
    }

    it.bad   = false;
    it.mtime = mt;
    it.size  = sz;
    it.info.w = img.width();
    it.info.h = img.height();
    it.full  = std::make_unique<gfx::Surface>(std::move(img));
    it.thumb.reset();                  // 缩略图过期了，下次 thumb() 重新生成
    ++reloads_;
    return true;
}

int AssetBank::check_disk() {
    int n = 0;
    for (int i = 0; i < count(); ++i) {
        Item& it = items_[size_t(i)];
        if (!it.full && !it.bad) continue;      // 没加载过的不用管

        int64_t sz = 0;
        const int64_t mt = mtime_of(root_ + "/" + it.info.rel, &sz);
        if (mt == 0) continue;                  // 文件暂时不在（正在被替换）

        const bool changed = (mt != it.mtime) || (sz != it.size);
        if (!changed && !it.bad) continue;

        if (reload(i)) ++n;
    }
    if (n > 0) std::printf("[assets] 热重载 %d 个文件\n", n);
    return n;
}

void AssetBank::poll(float dt) {
    poll_acc_ += dt;
    if (poll_acc_ < 0.4f) return;       // 每帧 stat 一千多次没必要
    poll_acc_ = 0.f;
    check_disk();
}

int AssetBank::loaded_count() const {
    int n = 0;
    for (const Item& it : items_) if (it.full) ++n;
    return n;
}

size_t AssetBank::memory_bytes() const {
    size_t n = 0;
    for (const Item& it : items_) {
        if (it.full)  n += size_t(it.full->width()) * size_t(it.full->height()) * sizeof(gfx::Pixel);
        if (it.thumb) n += size_t(it.thumb->width()) * size_t(it.thumb->height()) * sizeof(gfx::Pixel);
    }
    return n;
}

uint64_t AssetBank::stamp(int i) const {
    if (!valid(i)) return 0;
    const Item& it = items_[size_t(i)];
    // mtime 和 size 混在一起：任何一个变了，版本号就变。
    // 只用 mtime 会漏掉"同一秒里改完"的情况，只用 size 会漏掉"改像素不改大小"。
    return (uint64_t(it.mtime) << 1) ^ uint64_t(it.size) ^ (uint64_t(it.info.w) << 33);
}

} // namespace world
