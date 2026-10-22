#!/usr/bin/env python3
"""
LPC 角色合成器
==============
LPC (Liberated Pixel Cup) 的角色素材是「模块化」的：身体、头、头发、上衣、
裤子、鞋、武器分别存成独立的 spritesheet。本脚本按图层顺序把它们合成为
一张完整的角色 spritesheet，供游戏直接加载。

用法：
    python tools/build_sprites.py

输出帧规格：每帧 64x64，一行 = 一个朝向，一列 = 一帧动画。
朝向顺序（LPC 标准）：row0 = 朝上(背对), row1 = 朝左, row2 = 朝下(面向), row3 = 朝右
"""

import os
import sys
from PIL import Image

# ============================================================
# 路径配置
# ============================================================
# LPC 原始素材的根目录（下载自 https://github.com/ElizaWy/LPC）
# 如果你已经把 LPC 挪到别处，改这里
LPC_ROOT = os.environ.get(
    "LPC_ROOT",
    r"C:/Users/YangKai.Lin/AppData/Local/Temp/assets/lpc/Characters"
)
OUT_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                       "assets", "gfx", "chars")

# 需要合成的动画：脚本内部名 -> LPC 文件名
# 注意：LPC 的 Combat 动画帧布局不完整（部分方向缺失），本作改用
# walk/idle/run + 程序化攻击特效，所以不合成战斗动画。
ANIMS = {
    "idle": "Idle",
    "walk": "Walk",
    "run":  "Run",
}


# ============================================================
# 工具函数
# ============================================================
def first_existing_dir(*candidates):
    for c in candidates:
        if os.path.isdir(c):
            return c
    return None


def pick_color(base_dir, wanted, fallbacks=("Brown", "Black", "Blue")):
    """在配色目录里挑一个存在的颜色；都没有就用目录里第一个。"""
    if not os.path.isdir(base_dir):
        return None
    subs = [d for d in os.listdir(base_dir) if os.path.isdir(os.path.join(base_dir, d))]
    if wanted in subs:
        return wanted
    for f in fallbacks:
        if f in subs:
            return f
    return sorted(subs)[0] if subs else None


def load(*parts):
    p = os.path.join(LPC_ROOT, *parts)
    if not os.path.exists(p):
        return None
    return Image.open(p).convert("RGBA")


def load_layer(base, color, anim_file, skin=None, behind=False):
    """按 (目录, 颜色子目录, 动画文件名) 加载一层，自动处理有无配色两种布局。

    behind=True 时走 <base>/_Behind/<color>/<anim>.png，
    LPC 的长发分前后两层：后发要画在身体之前，前发画在头之后。
    """
    if base is None:
        return None
    root = os.path.join(LPC_ROOT, base)
    if behind:
        root = os.path.join(root, "_Behind")
    if skin:                                  # 身体/头：目录/肤色/动画.png
        im = load(root, skin, anim_file)
        if im is not None:
            return im
    if os.path.isdir(root):
        c = pick_color(root, color)
        if c:
            im = load(root, c, anim_file)
            if im is not None:
                return im
    return load(root, anim_file)              # 无配色子目录，直接取


def compose(layers):
    """把图层从底到顶合成到一张画布上（左上角对齐，LPC 是统一网格）。"""
    layers = [l for l in layers if l is not None]
    if not layers:
        return None
    w = max(l.width for l in layers)
    h = max(l.height for l in layers)
    canvas = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    for l in layers:
        canvas.alpha_composite(l, (0, 0))
    return canvas


# ============================================================
# 角色配置
# body/head 用肤色，clothing 用布料颜色
# ============================================================
BODY_M = "Body/Body 02 - Masculine, Thin"
BODY_F = "Body/Body 01 - Feminine, Thin"
HEAD_M = "Head/Head 02 - Masculine"
HEAD_F = "Head/Head 01 - Feminine"
HEAD_E = "Head/Head 03 - Elderly"

CLOTH = "Clothing/Masculine, Thin"
CLOTH_F = "Clothing/Feminine, Thin"

CHARACTERS = {
    # ---------------- 主角 ----------------
    "hero": dict(
        body=BODY_M, head=HEAD_M, skin="Tan",
        hair="Hair/Medium 03 - Idol", hair_color="Brown",
        brows="Hair/Eyebrows 01 - Thin Eyebrows", brows_color="Black",
        torso=f"{CLOTH}/Torso/Shirt 01 - Longsleeve Shirt", torso_color="Blue Violet",
        legs=f"{CLOTH}/Legs/Pants 03 - Pants", legs_color="Brown",
        feet=f"{CLOTH}/Feet/Shoes 02 - Boots", feet_color="Brown",
        weapon="Props/Sword 01 - Arming Sword", weapon_color="Bronze",
    ),
    # ---------------- NPC ----------------
    "elder": dict(
        body=BODY_M, head=HEAD_E, skin="Peach",
        hair="Hair/Short 06 - Balding", hair_color="White",
        brows="Hair/Eyebrows 02 - Thick Eyebrows", brows_color="White",
        facial="Hair/Facial Hair 07 - Medium Beard", facial_color="White",
        torso=f"{CLOTH}/Torso/Shirt 01 - Longsleeve Shirt", torso_color="Purple",
        legs=f"{CLOTH}/Legs/Pants 03 - Pants", legs_color="Black",
        feet=f"{CLOTH}/Feet/Shoes 01 - Shoes", feet_color="Black",
    ),
    "smith": dict(
        body=BODY_M, head=HEAD_M, skin="Tawny",
        hair="Hair/Short 01 - Buzzcut", hair_color="Black",
        brows="Hair/Eyebrows 02 - Thick Eyebrows", brows_color="Black",
        facial="Hair/Facial Hair 06 - Trimmed Beard", facial_color="Black",
        torso=f"{CLOTH}/Torso/Shirt 04 - T-shirt", torso_color="Brown",
        legs=f"{CLOTH}/Legs/Pants 05 - Overalls", legs_color="Charcoal",
        feet=f"{CLOTH}/Feet/Shoes 02 - Boots", feet_color="Black",
    ),
    "merchant": dict(
        body=BODY_F, head=HEAD_F, skin="Ivory",
        hair="Hair/Medium 08 - Bob, Bangs", hair_color="Blonde",
        brows="Hair/Eyebrows 01 - Thin Eyebrows", brows_color="Brown",
        torso=f"{CLOTH_F}/Torso/Shirt 01 - Longsleeve Shirt", torso_color="Green",
        legs=f"{CLOTH_F}/Legs/Pants 03 - Pants", legs_color="Brown",
        feet=f"{CLOTH_F}/Feet/Shoes 01 - Shoes", feet_color="Black",
    ),
    # ---------------- 敌人 ----------------
    "bandit": dict(
        body=BODY_M, head=HEAD_M, skin="Bronze",
        hair="Hair/Short 06 - Balding", hair_color="Black",
        brows="Hair/Eyebrows 02 - Thick Eyebrows", brows_color="Black",
        eyewear="Head Accessories/Eyewear 03 - Eyepatch", eyewear_color="Black",
        torso=f"{CLOTH}/Torso/Shirt 07 - Buttoned Longsleeve Shirt", torso_color="Charcoal",
        legs=f"{CLOTH}/Legs/Pants 03 - Pants", legs_color="Black",
        feet=f"{CLOTH}/Feet/Shoes 02 - Boots", feet_color="Black",
        weapon="Props/Sword 01 - Arming Sword", weapon_color="Bronze",
    ),
    "cultist": dict(
        body=BODY_M, head=HEAD_M, skin="Porcelain",
        hair=None,
        brows="Hair/Eyebrows 01 - Thin Eyebrows", brows_color="Black",
        helmet="Head Accessories/Helm 04 - Chainmail Hood", helmet_color="Bronze",
        torso=f"{CLOTH}/Torso/Shirt 01 - Longsleeve Shirt", torso_color="Purple",
        legs=f"{CLOTH}/Legs/Pants 03 - Pants", legs_color="Purple",
        feet=f"{CLOTH}/Feet/Shoes 01 - Shoes", feet_color="Black",
    ),
    "knight": dict(
        body=BODY_M, head=HEAD_M, skin="Tan",
        hair=None,
        brows=None,
        helmet="Head Accessories/Helm 02 - Bascinet, Closed", helmet_color="Bronze",
        torso=f"{CLOTH}/Torso/Shirt 09 - Polo", torso_color="Charcoal",
        legs=f"{CLOTH}/Legs/Pants 01 - Hose", legs_color="Charcoal",
        feet=f"{CLOTH}/Feet/Shoes 02 - Boots", feet_color="Black",
        weapon="Props/Sword 01 - Arming Sword", weapon_color="Brass",
    ),
}


def build_one(name, cfg):
    os.makedirs(OUT_DIR, exist_ok=True)
    made = []
    for key, anim_file in ANIMS.items():
        f = f"{anim_file}.png"
        layers = [
            load_layer(cfg.get("hair"), cfg.get("hair_color"), f, behind=True),  # 后发
            load_layer(cfg["body"],  None,        f, skin=cfg["skin"]),   # 身体（皮肤）
            load_layer(cfg.get("legs"),  cfg.get("legs_color"),  f),
            load_layer(cfg.get("feet"),  cfg.get("feet_color"),  f),
            load_layer(cfg.get("torso"), cfg.get("torso_color"), f),
            load_layer(cfg["head"],  None,        f, skin=cfg["skin"]),   # 头
            load_layer(cfg.get("hair"),   cfg.get("hair_color"),   f),    # 前发
            load_layer(cfg.get("brows"),  cfg.get("brows_color"),  f),
            load_layer(cfg.get("facial"), cfg.get("facial_color"), f),
            load_layer(cfg.get("eyewear"), cfg.get("eyewear_color"), f),
            load_layer(cfg.get("helmet"),  cfg.get("helmet_color"),  f),
            load_layer(cfg.get("weapon"), cfg.get("weapon_color"), f),
        ]
        out = compose(layers)
        if out is None:
            print(f"  [skip] {name}/{key}: 没有可用图层")
            continue
        path = os.path.join(OUT_DIR, f"{name}_{key}.png")
        out.save(path)
        made.append((key, out.width, out.height, path))
    return made


def make_preview(paths, out_png):
    """把各角色 walk 的第一行四帧并排输出，用来肉眼检查合成结果。"""
    rows = []
    maxw = 0
    for name in paths:
        p = os.path.join(OUT_DIR, f"{name}_walk.png")
        if not os.path.exists(p):
            continue
        im = Image.open(p).convert("RGBA")
        fw, fh = im.width // 8, im.height // 4
        strip = Image.new("RGBA", (fw * 4, fh), (0, 0, 0, 0))
        for d in range(4):
            strip.alpha_composite(im.crop((0, d * fh, fw, (d + 1) * fh)), (d * fw, 0))
        rows.append((name, strip))
        maxw = max(maxw, strip.width)
    if not rows:
        return
    H = sum(r.height for _, r in rows) + 4 * len(rows)
    canvas = Image.new("RGBA", (maxw + 80, H), (30, 30, 34, 255))
    y = 0
    for name, strip in rows:
        canvas.alpha_composite(strip, (80, y))
        y += strip.height + 4
    canvas.save(out_png)
    print("预览图:", out_png)


if __name__ == "__main__":
    if not os.path.isdir(LPC_ROOT):
        sys.exit(f"找不到 LPC 素材目录: {LPC_ROOT}\n请设置环境变量 LPC_ROOT。")
    total = 0
    for name, cfg in CHARACTERS.items():
        made = build_one(name, cfg)
        for key, w, h, p in made:
            frames = w // 64
            dirs = h // 64
            print(f"  {name}/{key:8s} {w}x{h}  ({frames} 帧 x {dirs} 朝向)  -> {os.path.basename(p)}")
        total += len(made)
    print(f"共生成 {total} 张 spritesheet -> {OUT_DIR}")
    make_preview([n for n in CHARACTERS],
                 os.path.join(OUT_DIR, "_preview.png"))
