#include "world/TileMap.h"
#include "gfx/Atlas.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>   // std::error_code

namespace world {

namespace fs = std::filesystem;

// 去掉行尾的 \r（Windows 换行）。不做的话 bg 那行最后会多一个 \r，
// 导致文件名对不上，报"文件不存在"但看起来完全一样。
static void strip_cr(std::string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) s.pop_back();
}

TileMap TileMap::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::fprintf(stderr, "打不开地图: %s\n", path.c_str());
        std::abort();
    }

    TileMap m;

    enum class Sec { Header, Ground, Solid, Decor, Entity };
    Sec sec = Sec::Header;

    std::string line;
    while (std::getline(in, line)) {
        strip_cr(line);
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        // 段标记
        if (line[0] == '[') {
            if (line == "[GROUND]") sec = Sec::Ground;
            else if (line == "[SOLID]") sec = Sec::Solid;
            else if (line == "[DECOR]") sec = Sec::Decor;
            else if (line == "[ENTITY]") sec = Sec::Entity;
            continue;
        }

        if (sec == Sec::Header) {
            std::istringstream ss(line);
            std::string key;
            ss >> key;

            if (key == "name")         { ss >> m.name; }
            else if (key == "tileset") { ss >> m.tileset; }
            else if (key == "weather") { ss >> m.weather; }
            else if (key == "size")    { ss >> m.w >> m.h; }
            else if (key == "bg") {
                // bg 的值可能含空格（DemoGame - 2 - Summer.png），
                // 不能用 ss >> bg。跳过 key 之后，剩下整行都是值。
                std::string rest;
                std::getline(ss, rest);
                const size_t p = rest.find_first_not_of(" \t");
                m.bg = (p == std::string::npos) ? "" : rest.substr(p);
            }
            continue;
        }

        if (sec == Sec::Ground) {
            std::istringstream ss(line);
            int v;
            while (ss >> v) m.ground.push_back(v);
            continue;
        }

        if (sec == Sec::Solid) {
            std::istringstream ss(line);
            int v;
            while (ss >> v) m.solid.push_back(uint8_t(v));
            continue;
        }

        if (sec == Sec::Decor) {
            std::istringstream ss(line);
            int v;
            while (ss >> v) m.decor.push_back(uint8_t(v));
            continue;
        }

        // Entity
        {
            std::istringstream ss(line);
            EntitySpawn e;
            ss >> e.type >> e.x >> e.y;
            if (e.type == "portal") ss >> e.target_map >> e.tx >> e.ty;
            m.spawns.push_back(e);
        }
    }

    // 校验：数据量必须刚好等于 w*h
    const size_t need = size_t(m.w) * size_t(m.h);
    if (need == 0 || m.ground.size() != need || m.solid.size() != need) {
        std::fprintf(stderr,
            "地图数据不完整: %s\n  期望 %zu 格, ground=%zu solid=%zu\n",
            path.c_str(), need, m.ground.size(), m.solid.size());
        std::abort();
    }

    // 装饰层是后加的，老地图没有这一段 —— 补成空，不算错误。
    // 但如果有，就必须正好是 w*h，否则是导出漏了数据。
    if (!m.decor.empty() && m.decor.size() != need) {
        std::fprintf(stderr,
            "装饰层格数不对: %s\n  期望 %zu 格, decor=%zu\n",
            path.c_str(), need, m.decor.size());
        std::abort();
    }
    if (m.decor.empty()) m.decor.assign(need, 0);

    // 图集：先 terrain/，再 backgrounds/
    std::error_code ec;
    fs::path p = fs::path("assets/gfx/terrain") / m.tileset;
    if (!fs::exists(p, ec)) p = fs::path("assets/gfx/backgrounds") / m.tileset;
    if (!fs::exists(p, ec)) {
        std::fprintf(stderr, "找不到图集: %s\n", m.tileset.c_str());
        std::abort();
    }

    m.image = gfx::load_png(p.string());
    m.tpr = m.image.width() / TS;
    if (m.tpr <= 0) {
        std::fprintf(stderr, "图集宽度不是 %d 的倍数: %s\n", TS, m.tileset.c_str());
        std::abort();
    }

    std::printf("[map] %-8s %dx%d  tpr=%d  entities=%zu\n",
                m.name.c_str(), m.w, m.h, m.tpr, m.spawns.size());
    return m;
}

// ---------------------------------------------------------------
void TileMap::fill(int x0, int y0, int x1, int y1, int index) {
    if (x0 > x1) { const int t = x0; x0 = x1; x1 = t; }
    if (y0 > y1) { const int t = y0; y0 = y1; y1 = t; }
    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x)
            paint(x, y, index);
}

bool TileMap::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        std::fprintf(stderr, "写地图失败（打不开）: %s\n", path.c_str());
        return false;
    }

    // 头部的顺序和格式要和 load 一致，否则来回存读会漂移
    out << "# SIMDRPG map v1\n";
    out << "name    " << name    << '\n';
    out << "tileset " << tileset << '\n';
    out << "bg      " << bg      << '\n';      // bg 可能为空，留一行空的没事
    out << "weather " << weather << '\n';
    out << "size    " << w << ' ' << h << '\n';

    out << "[GROUND]\n";
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (x) out << ' ';
            out << at(x, y);
        }
        out << '\n';
    }

    out << "[SOLID]\n";
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (x) out << ' ';
            out << int(solid[size_t(y) * size_t(w) + size_t(x)]);
        }
        out << '\n';
    }

    out << "[DECOR]\n";
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (x) out << ' ';
            out << decor_at(x, y);
        }
        out << '\n';
    }

    out << "[ENTITY]\n";
    for (const auto& e : spawns) {
        out << e.type << ' ' << e.x << ' ' << e.y;
        if (e.type == "portal")
            out << ' ' << e.target_map << ' ' << e.tx << ' ' << e.ty;
        out << '\n';
    }

    if (!out) {
        std::fprintf(stderr, "写地图失败（写一半出错）: %s\n", path.c_str());
        return false;
    }
    std::printf("[map] 已保存 %s  (%dx%d, %zu 个实体)\n",
                path.c_str(), w, h, spawns.size());
    return true;
}

bool TileMap::set_tileset(const std::string& file) {
    // 用 error_code 重载：不带它的 exists() 在路径转换失败时**会抛异常**，
    // 而这里调用方（编辑器）不该被一个打错的文件名炸掉。
    // 中文路径尤其容易踩：/utf-8 下窄字符串是 UTF-8，filesystem 却按 GBK 解释。
    std::error_code ec;

    fs::path p = fs::path("assets/gfx/terrain") / file;
    if (!fs::exists(p, ec)) p = fs::path("assets/gfx/backgrounds") / file;
    if (!fs::exists(p, ec)) {
        std::fprintf(stderr, "换图集失败，找不到: %s\n", file.c_str());
        return false;
    }

    gfx::Surface img = gfx::load_png(p.string());
    const int cols = img.width() / TS;
    if (cols <= 0) {
        std::fprintf(stderr, "图集宽度不对: %s\n", file.c_str());
        return false;
    }

    image   = std::move(img);
    tpr     = cols;
    tileset = file;
    return true;
}

} // namespace world
