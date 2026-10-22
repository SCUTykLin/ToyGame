#!/usr/bin/env python3
"""终审脚本：多维度检查仓库一致性。

检查项：
  1. 讲义引用的素材路径是否存在
  2. 讲义里的敌人名是否都在 ENEMIES[] 里（反之亦然）
  3. ENEMIES[] 里的素材文件是否存在、帧宽是否超图
  4. 地图里的敌人名是否都在 ENEMIES[] 里
  5. 讲义的 FILE 标记 vs 磁盘文件（由 extract_code --check 负责）
  6. 文档里的数字陈述（37 个文件 / 28 种敌人 / 6 张图）
  7. 占位符 / 未定义常量
"""
import glob
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def png_size(path):
    try:
        with open(path, "rb") as f:
            head = f.read(33)
        if head[:8] != b"\x89PNG\r\n\x1a\n":
            return None
        return struct.unpack(">II", head[16:24])
    except OSError:
        return None


def main():
    issues = []
    warns = []

    docs = sorted(glob.glob(os.path.join(ROOT, "docs", "*.md")))
    readme = os.path.join(ROOT, "README.md")
    texts = {}
    for p in docs + [readme]:
        texts[os.path.basename(p)] = open(p, encoding="utf-8").read()
    all_text = "\n".join(texts.values())

    # ---- 1. 素材路径 ----
    pat = re.compile(r"`(assets/[^`]+?\.(?:png|map|txt|md))`")
    missing = set()
    for m in pat.findall(all_text):
        if any(c in m for c in "*<>"):
            continue
        if not os.path.exists(os.path.join(ROOT, m)):
            missing.add(m)
    if missing:
        for m in sorted(missing):
            issues.append(f"素材路径不存在: {m}")

    # ---- 2. ENEMIES[] 表 ----
    enemy_cpp = open(os.path.join(ROOT, "verify/full/src/game/Enemy.cpp"),
                     encoding="utf-8").read()
    rows = re.findall(r'^\s*\{"([a-z_]+)",\s*"([^"]+)",\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*'
                      r'(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)',
                      enemy_cpp, re.M)
    names = [r[0] for r in rows]
    if not rows:
        issues.append("ENEMIES[] 解析失败（正则不匹配）")

    # 素材存在 + 帧宽
    for name, sprite, hp, atk, spd, fw, fh, frames, rows_n, dw, dh in rows:
        p = os.path.join(ROOT, "assets/gfx", sprite)
        if not os.path.exists(p):
            issues.append(f"{name} 的素材不存在: {sprite}")
            continue
        sz = png_size(p)
        if not sz:
            issues.append(f"{name} 的素材不是合法 PNG: {sprite}")
            continue
        w, h = sz
        fw_i, frames_i = int(fw), int(frames)
        if fw_i * frames_i > w:
            issues.append(f"{name}: fw({fw_i}) × frames({frames_i}) > 图宽 {w}")
        if int(fh) > h:
            issues.append(f"{name}: fh({fh}) > 图高 {h}")
        # draw 尺寸合理性
        if int(dw) <= 0 or int(dh) <= 0:
            issues.append(f"{name}: draw_w/h 必须 > 0")
        if int(dw) > 200 or int(dh) > 200:
            warns.append(f"{name}: draw {dw}x{dh} 偏大（屏幕 640x360）")

    # 文档里出现的 enemy 名
    doc00 = texts.get("00-世界观与六张地图.md", "")
    doc_names = set(re.findall(r"^\| \d+ \| `([a-z_]+)` \|", doc00, re.M))
    doc_names |= set(re.findall(r"`(boss_(?!defeated)[a-z]+)`", doc00))
    missing_in_code = doc_names - set(names)
    if missing_in_code:
        issues.append("00 章表格里有、代码里没有: " + ", ".join(sorted(missing_in_code)))
    missing_in_doc = set(names) - doc_names
    if missing_in_doc:
        issues.append("代码里有、00 章表格没有: " + ", ".join(sorted(missing_in_doc)))

    # ---- 3. 地图里的敌人 ----
    map_enemies = set()
    for mp in glob.glob(os.path.join(ROOT, "assets/maps/*.map")):
        t = open(mp, encoding="utf-8").read()
        sec = t.split("[ENTITY]")[-1]
        for line in sec.strip().split("\n"):
            parts = line.split()
            if not parts:
                continue
            typ = parts[0]
            if typ.startswith("enemy_"):
                map_enemies.add(typ[len("enemy_"):])
            elif typ.startswith("boss_"):
                map_enemies.add(typ)
    unknown = map_enemies - set(names)
    if unknown:
        issues.append("地图引用了不存在的敌人: " + ", ".join(sorted(unknown)))

    # ---- 4. 数字陈述 ----
    n_src = len(glob.glob(os.path.join(ROOT, "verify/full/src/**/*.cpp"), recursive=True))
    n_hdr = len(glob.glob(os.path.join(ROOT, "verify/full/include/**/*.h"), recursive=True))
    total_files = n_src + n_hdr

    if "37 个文件" in all_text and total_files != 37:
        warns.append(f"文档说 37 个文件，实际 {total_files} 个（{n_hdr} h + {n_src} cpp）")
    if "28 种敌人" in all_text and len(names) != 28:
        issues.append(f"文档说 28 种敌人，ENEMIES[] 实际 {len(names)} 行")
    n_maps = len(glob.glob(os.path.join(ROOT, "assets/maps/*.map")))
    if n_maps != 6:
        issues.append(f"地图应为 6 张，实际 {n_maps}")

    n_png = sum(1 for r, _, fs in os.walk(os.path.join(ROOT, "assets/gfx"))
                for f in fs if f.lower().endswith(".png") and not f.startswith("_contact"))
    if "1195" in all_text and n_png != 1195:
        warns.append(f"文档说 1195 张素材，实际 {n_png}")

    # ---- 5. 占位符 ----
    for fn, t in texts.items():
        for p in ("TODO", "待补", "FIXME", "???"):
            if p in t:
                issues.append(f"{fn} 含占位符 {p}")

    # 未定义常量（B_XXX 旧写法）
    for n in sorted(set(re.findall(r"\bB_[A-Z_]+\b", all_text))):
        issues.append(f"残留旧常量 {n}")

    # ---- 输出 ----
    print("=" * 66)
    print(f"文件: {n_hdr} 头 + {n_src} 源 = {total_files}")
    print(f"敌人: {len(names)} 行   地图: {n_maps}   素材: {n_png}")
    print("=" * 66)
    if issues:
        print(f"\n[问题] {len(issues)} 条")
        for i in issues:
            print("  -", i)
    if warns:
        print(f"\n[提醒] {len(warns)} 条")
        for w in warns:
            print("  -", w)
    if not issues and not warns:
        print("\n全部通过。")
    return 1 if issues else 0


if __name__ == "__main__":
    sys.exit(main())
