#!/usr/bin/env python3
"""
素材拼图（contact sheet）生成器
================================
assets/gfx 下有 600+ 个 PNG，一个个打开看会死人。
本脚本把每个目录的素材拼成一张大图，缩略图按网格排列并标注文件名，
你打开大图扫一眼就能决定"这个用在哪"。

输出：assets/gfx/_contact_<目录名>.png
用法：python tools/make_contact_sheet.py
"""

import os
import math
from PIL import Image, ImageDraw

BASE = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                    "assets", "gfx")

THUMB = 96          # 缩略图边长
COLS = 8
BG = (28, 28, 32, 255)
FG = (230, 230, 235)
LABEL_H = 16


def slugify(name):
    return name.replace("/", "_").replace(" ", "_").replace(".png", "")


def build_sheet(dirname, files, out_dir):
    if not files:
        return
    rows = math.ceil(len(files) / COLS)
    W = COLS * THUMB
    H = rows * (THUMB + LABEL_H)
    sheet = Image.new("RGBA", (W, H), BG)
    draw = ImageDraw.Draw(sheet)

    for i, f in enumerate(files):
        path = os.path.join(dirname, f)
        try:
            im = Image.open(path).convert("RGBA")
        except Exception:
            continue
        # 等比缩放塞进缩略图格子（不拉伸）
        scale = min(THUMB / max(im.width, 1), THUMB / max(im.height, 1))
        if scale > 1:      # 小图不要放大太多，保持像素感
            scale = min(scale, 3.0)
        nw, nh = max(1, int(im.width * scale)), max(1, int(im.height * scale))
        thumb = im.resize((nw, nh), Image.NEAREST)

        cx = (i % COLS) * THUMB
        cy = (i // COLS) * (THUMB + LABEL_H)
        # 居中贴
        sheet.alpha_composite(thumb, (cx + (THUMB - nw) // 2,
                                      cy + (THUMB - nh) // 2))
        label = f if len(f) <= 14 else f[:11] + "..."
        draw.text((cx + 3, cy + THUMB + 2), label, fill=FG)

    out = os.path.join(out_dir, f"_contact_{slugify(os.path.basename(dirname))}.png")
    sheet.save(out)
    print(f"  {os.path.basename(dirname):14s} {len(files):4d} 个 -> "
          f"{os.path.basename(out)}  ({sheet.width}x{sheet.height})")


def main():
    print(f"扫描 {BASE}")
    for d in sorted(os.listdir(BASE)):
        path = os.path.join(BASE, d)
        if not os.path.isdir(path):
            continue
        files = sorted(f for f in os.listdir(path) if f.lower().endswith(".png"))
        build_sheet(path, files, BASE)
        # 二级目录（如 chars/extra/ninja）也各出一张
        for sub in sorted(os.listdir(path)):
            spath = os.path.join(path, sub)
            if not os.path.isdir(spath):
                continue
            sfiles = sorted(f for f in os.listdir(spath) if f.lower().endswith(".png"))
            if sfiles:
                build_sheet(spath, sfiles, BASE)
            for sub2 in sorted(os.listdir(spath)):
                s2 = os.path.join(spath, sub2)
                if os.path.isdir(s2):
                    f2 = sorted(f for f in os.listdir(s2) if f.lower().endswith(".png"))
                    if f2:
                        build_sheet(s2, f2, BASE)


if __name__ == "__main__":
    main()
