#!/usr/bin/env python3
"""
SIMDRPG 地图生成器
==================
生成 6 张地图，输出人类可读的 .map。

地图格式（v2，加了装饰层）
--------------------------
    # SIMDRPG map v1
    name    village
    tileset terrain_summer.png
    bg      DemoGame - 2 - Summer.png
    size    64 48
    weather clear
    [GROUND]
    17 17 37 ...            <- 每格的地形 tile
    [SOLID]
    0 0 1 ...               <- 每格 0/1，1 = 挡路
    [DECOR]
    0 0 12 0 ...            <- 每格装饰物编号，0 = 空，n = PROPS[n-1]
    [ENTITY]
    npc_elder 20 15

设计原则
--------
1. 地形管"这是什么地面"，装饰管"这里有什么东西"。
   房子的墙是地形，房子旁边的桶是装饰。
2. 装饰物锚在"底部中点"。一棵 187px 的树从锚点往上长 5 格，
   所以整棵树只占一个格子，却挡住上面 5 格 —— 这就是"树能遮住人"。
3. 碰撞跟着装饰走。solid 的树/桶/墓碑会自动把那一格设成墙。
4. 别把装饰放在实体格子上，会挡住 NPC / 传送门。
"""

import os
import math
import random

W, H = 64, 48
OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "assets", "maps")

# ---------------------------------------------------------------
# 地形 tile 索引（由颜色分析定位，四季图集通用）
# ---------------------------------------------------------------
T_GROUND      = 17      # 纯地面（夏=草 / 秋=枯草 / 冬=雪）
T_GROUND_ALT  = 20
T_GROUND_ALT2 = 21
T_WATER       = 177
T_SAND        = 26      # 沙土 / 路
T_STONE       = 250     # 石地 / 石墙
T_WOOD        = 251     # 木板 / 屋顶（棕色，和灰墙一搭就是房子）
T_GRASS_TUFT  = 134     # 草丛地被（亮绿、高纹理）

# rift 用 ninja 图集（12x10 = 120 个 tile），索引不能沿用上面那套
T_RIFT_VOID     = 64
T_RIFT_PLATFORM = 116

# ---------------------------------------------------------------
# 装饰物编号 —— 必须和 verify/full/src/world/Props.cpp 的 kProps[] 一一对应
# （地图里存 1-based，C++ 里减 1 取表）
# ---------------------------------------------------------------
P_TREE1     = 1
P_TREE2     = 2
P_BUSH      = 3
P_ROLLBUSH  = 4
P_BRANCH    = 5
P_CACTUS1   = 6
P_CACTUS2   = 7
P_CACTUS3   = 8
P_ROCK4     = 9
P_ROCK5     = 10
P_ROCK6     = 11
P_BARREL    = 12
P_EXPLOSIVE = 13
P_CRATE1    = 14
P_CRATE2    = 15
P_JAR1      = 16
P_JAR2      = 17
P_TROUGH    = 18
P_CHARIOT   = 19
P_LADDER    = 20
P_BARRIER   = 21
P_CHEST     = 22
P_SIGN1     = 23
P_SIGN2     = 24
P_FIRE      = 25
P_GRAVE1    = 26
P_GRAVE2    = 27
P_GRAVE3    = 28
P_SKULL     = 29
P_BONE      = 30
P_PIANO     = 31
P_METEOR    = 32
P_PLANET    = 33

# 哪些装饰会挡路（和 C++ 表里的 solid 一致）
P_SOLID = {P_TREE1, P_TREE2, P_CACTUS1, P_CACTUS3, P_ROCK4,
           P_BARREL, P_EXPLOSIVE, P_CRATE1, P_CRATE2, P_TROUGH, P_CHARIOT,
           P_BARRIER, P_CHEST, P_SIGN1, P_SIGN2,
           P_GRAVE1, P_GRAVE2, P_GRAVE3, P_PIANO, P_METEOR}


def _hash01(ix, iy, seed):
    """把 (格坐标, seed) 打成 0..1 的伪随机数。"""
    n = (ix * 374761393 + iy * 668265263 + seed * 1013904223) & 0xFFFFFFFF
    n = ((n ^ (n >> 13)) * 1274126177) & 0xFFFFFFFF
    return ((n ^ (n >> 16)) & 0xFFFF) / 65535.0


def noise2(x, y, seed=0, scale=4.0):
    """平滑 value noise：双线性插值 + 平滑曲线。

    早先的版本用 int(x * 0.12) 做格点，导致 8×8 格才有一次变化 ——
    640×360 的视野正好落在一两个块里，整屏就是一片纯色。
    现在 scale=4，视野内有 5×3 个斑块，地形才有"起伏"。
    """
    fx, fy = x / scale, y / scale
    ix, iy = int(fx), int(fy)
    tx, ty = fx - ix, fy - iy
    tx = tx * tx * (3 - 2 * tx)
    ty = ty * ty * (3 - 2 * ty)

    a = _hash01(ix,     iy,     seed)
    b = _hash01(ix + 1, iy,     seed)
    c = _hash01(ix,     iy + 1, seed)
    d = _hash01(ix + 1, iy + 1, seed)

    return (a * (1 - tx) + b * tx) * (1 - ty) + (c * (1 - tx) + d * tx) * ty


def new_map():
    return {
        "ground": [[T_GROUND] * W for _ in range(H)],
        "solid":  [[0] * W for _ in range(H)],
        "decor":  [[0] * W for _ in range(H)],
        "ents":   [],
    }


def fill_noise(g, base, variants, threshold=0.55):
    """给地面撒变体，免得整片纯色。"""
    for y in range(H):
        for x in range(W):
            n = noise2(x, y, 3)
            g[y][x] = variants[int(n * len(variants)) % len(variants)] if n > threshold else base


def road(g, s, x0, y0, x1, y1, tile=T_SAND, clear=True):
    for y in range(max(0, y0), min(H, y1 + 1)):
        for x in range(max(0, x0), min(W, x1 + 1)):
            g[y][x] = tile
            if clear:
                s[y][x] = 0


def put(m, x, y, prop, force_solid=None):
    """放一个装饰物。越界忽略；已有装饰不覆盖；solid 的自动设碰撞。"""
    if x < 0 or y < 0 or x >= W or y >= H:
        return False
    if m["decor"][y][x] != 0:
        return False
    m["decor"][y][x] = prop
    should = (prop in P_SOLID) if force_solid is None else force_solid
    if should:
        m["solid"][y][x] = 1
    return True


def free(m, x, y, margin=1):
    """这块地能不能放装饰。"""
    if x < 0 or y < 0 or x >= W or y >= H:
        return False
    if m["decor"][y][x] != 0:
        return False
    for e in m["ents"]:
        ex, ey = int(e[1]), int(e[2])
        if abs(ex - x) <= margin and abs(ey - y) <= margin:
            return False
    return True


def scatter(m, rnd, props, count, tries=4000, avoid=None, keep_clear=None):
    """在空旷处随机撒装饰。props 可以是单个编号或列表。"""
    if isinstance(props, int):
        props = [props]
    placed = 0
    for _ in range(tries):
        if placed >= count:
            break
        x, y = rnd.randrange(2, W - 2), rnd.randrange(2, H - 2)
        if not free(m, x, y):
            continue
        if m["solid"][y][x]:
            continue
        if avoid:
            ax, ay, ar = avoid
            if abs(x - ax) <= ar and abs(y - ay) <= ar:
                continue
        if keep_clear and keep_clear(x, y):
            continue
        put(m, x, y, rnd.choice(props))
        placed += 1
    return placed


def house(m, x0, y0, w=7, h=4, door_offset=3):
    """房子 = 一圈石墙 + 中间木屋顶。

    只铺"一圈石墙"的话，视觉上是个灰色方框，不像建筑。
    中间填木板（T_WOOD）当屋顶，才有"这是个房子"的体积感。
    整栋不可走 —— 玩家进不去室内，但也不需要。
    """
    g, s = m["ground"], m["solid"]
    for y in range(y0, y0 + h):
        for x in range(x0, x0 + w):
            if x < 0 or y < 0 or x >= W or y >= H:
                continue
            edge = (y == y0 or y == y0 + h - 1 or x == x0 or x == x0 + w - 1)
            g[y][x] = T_STONE if edge else T_WOOD
            s[y][x] = 1

    dx = x0 + door_offset
    dy = y0 + h - 1
    if 0 <= dx < W and 0 <= dy < H:
        g[dy][dx] = T_SAND
        s[dy][dx] = 0
        for yy in range(dy + 1, min(H, dy + 4)):
            g[yy][dx] = T_SAND


# ---------------------------------------------------------------
# 1) 晨曦村
# ---------------------------------------------------------------
def gen_village():
    m = new_map()
    g, s, d = m["ground"], m["solid"], m["decor"]
    rnd = random.Random(1)

    fill_noise(g, T_GROUND, [T_GROUND, T_GROUND_ALT, T_GROUND_ALT2], 0.5)

    # 十字主路
    road(g, s, 30, 4, 33, 43)
    road(g, s, 4, 22, 59, 25)

    # 池塘
    for y in range(7, 15):
        for x in range(8, 17):
            if (x - 12) ** 2 + (y - 11) ** 2 <= 14:
                g[y][x] = T_WATER
                s[y][x] = 1

    # 草丛地被：在草地上撒 134，打散大片纯绿
    for y in range(2, H - 2):
        for x in range(2, W - 2):
            if g[y][x] == T_GROUND and noise2(x, y, 21) > 0.62:
                g[y][x] = T_GRASS_TUFT

    # 六间房子（沿主路排布，不是四角各一栋）
    house(m, 12, 12, 8, 5, 4)
    house(m, 40, 10, 8, 5, 4)
    house(m, 12, 32, 8, 5, 4)
    house(m, 44, 33, 8, 5, 4)
    house(m, 22, 12, 6, 4, 3)
    house(m, 22, 34, 6, 4, 3)

    m["ents"] = [
        ("npc_elder",    15, 19),
        ("npc_smith",    47, 19),
        ("npc_merchant", 15, 39),
        ("portal",       62, 24, "forest", 3, 24),
    ]

    # 广场
    put(m, 31, 27, P_FIRE)
    put(m, 28, 27, P_CRATE1)
    put(m, 35, 27, P_BARREL)
    put(m, 29, 20, P_SIGN1)
    put(m, 34, 30, P_TROUGH)

    # 房子周围杂物（堆得多才像有人住）
    for hx, hy in [(12, 12), (40, 10), (12, 32), (44, 33), (22, 12), (22, 34)]:
        for _ in range(10):
            px = hx + rnd.randrange(-2, 10)
            py = hy + rnd.randrange(5, 9)
            if free(m, px, py):
                put(m, px, py, rnd.choice([P_BARREL, P_CRATE1, P_CRATE2, P_JAR1, P_JAR2,
                                           P_BARREL, P_CRATE1]))

    # 村口栅栏 + 板车
    for x in range(56, 62):
        if free(m, x, 26):
            put(m, x, 26, P_BARRIER)
    put(m, 52, 36, P_CHARIOT)

    # 边界树林
    for y in range(H):
        for x in range(W):
            if x < 3 or x > W - 4 or y < 3 or y > H - 4:
                if rnd.random() < 0.5 and free(m, x, y):
                    put(m, x, y, P_TREE1 if rnd.random() < 0.7 else P_TREE2)

    # 村内植被
    scatter(m, rnd, [P_TREE1, P_TREE2], 34, avoid=(31, 24, 4))
    scatter(m, rnd, P_BUSH, 46)
    scatter(m, rnd, P_ROLLBUSH, 20)
    scatter(m, rnd, P_ROCK5, 14)

    return dict(name="village", tileset="terrain_summer.png",
                bg="DemoGame - 2 - Summer.png", weather="clear", m=m)


# ---------------------------------------------------------------
# 2) 低语森林
# ---------------------------------------------------------------
def gen_forest():
    m = new_map()
    g, s, d = m["ground"], m["solid"], m["decor"]
    rnd = random.Random(2)

    fill_noise(g, T_GROUND, [T_GROUND, T_GROUND_ALT], 0.45)

    # 蜿蜒小径
    for x in range(3, 61):
        cy = int(24 + 5 * math.sin(x * 0.18))
        for dy in (-1, 0, 1):
            yy = cy + dy
            if 0 <= yy < H:
                g[yy][x] = T_SAND
                s[yy][x] = 0

    m["ents"] = [
        ("enemy_bandit",  14, 20),
        ("enemy_bandit",  22, 30),
        ("enemy_bandit",  40, 18),
        ("enemy_cultist", 34, 34),
        ("enemy_cultist", 50, 22),
        ("enemy_knight",  55, 30),
        ("portal",         1, 24, "village", 60, 24),
        ("portal",        61, 24, "pass", 4, 24),
    ]

    # 密林
    for y in range(3, H - 3):
        for x in range(3, W - 3):
            if not free(m, x, y, margin=0):
                continue
            near_path = False
            for dx in range(-2, 3):
                xx = x + dx
                if 0 <= xx < W and g[y][xx] == T_SAND:
                    near_path = True
                    break
            if near_path:
                continue
            if rnd.random() < 0.52:
                put(m, x, y, P_TREE1 if rnd.random() < 0.75 else P_TREE2)

    # 林下
    scatter(m, rnd, P_BUSH, 110)
    scatter(m, rnd, P_ROLLBUSH, 55)
    scatter(m, rnd, [P_ROCK4, P_ROCK5, P_ROCK6], 25)
    scatter(m, rnd, P_BRANCH, 22)
    scatter(m, rnd, P_BONE, 10)
    scatter(m, rnd, P_METEOR, 3)

    # 林间遗迹
    for x in range(46, 51):
        g[38][x] = T_STONE
        s[38][x] = 1
    for y in range(35, 39):
        g[y][46] = T_STONE
        s[y][46] = 1
    put(m, 48, 37, P_CHEST)
    put(m, 47, 37, P_JAR1)

    return dict(name="forest", tileset="terrain_autumn.png",
                bg="DemoGame - 3 - Autumn.png", weather="rain", m=m)


# ---------------------------------------------------------------
# 3) 霜噬山道
# ---------------------------------------------------------------
def gen_pass():
    m = new_map()
    g, s, d = m["ground"], m["solid"], m["decor"]
    rnd = random.Random(3)

    fill_noise(g, T_GROUND, [T_GROUND, T_GROUND_ALT], 0.4)

    # 冰河
    for y in range(H):
        cx = int(32 + 4 * math.sin(y * 0.22))
        for dx in range(-2, 3):
            x = cx + dx
            if 0 <= x < W:
                g[y][x] = T_WATER
                s[y][x] = 1

    # 两座栈桥
    for y in (16, 34):
        for x in range(25, 40):
            g[y][x] = T_SAND
            s[y][x] = 0

    # 两侧山壁
    for y in range(H):
        for x in range(W):
            if rnd.random() < 0.3 and (x < 10 or x > W - 11):
                if s[y][x] == 0:
                    g[y][x] = T_STONE
                    s[y][x] = 1

    m["ents"] = [
        ("enemy_cultist", 16, 18),
        ("enemy_cultist", 48, 30),
        ("enemy_knight",  22, 40),
        ("enemy_knight",  50, 12),
        ("enemy_bandit",  40, 26),
        ("portal",         2, 24, "forest", 60, 24),
        ("portal",        62, 24, "crypt", 4, 24),
    ]

    scatter(m, rnd, [P_ROCK4, P_ROCK5, P_ROCK6], 90)
    scatter(m, rnd, P_TREE1, 16)
    scatter(m, rnd, P_BRANCH, 12)
    scatter(m, rnd, P_BONE, 6)

    put(m, 12, 22, P_SIGN1)
    put(m, 52, 26, P_SIGN2)

    return dict(name="pass", tileset="terrain_winter.png",
                bg="DemoGame - 4 - Winter.png", weather="snow", m=m)


# ---------------------------------------------------------------
# 4) 灰烬地窖
# ---------------------------------------------------------------
def gen_crypt():
    m = new_map()
    g, s, d = m["ground"], m["solid"], m["decor"]
    rnd = random.Random(4)

    for y in range(H):
        for x in range(W):
            g[y][x] = T_STONE

    road(g, s, 2, 22, 61, 25, T_SAND)

    # 三间石室
    rooms = [(8, 6, 14, 12), (26, 6, 12, 10), (8, 30, 12, 14)]
    for (rx, ry, rw, rh) in rooms:
        for y in range(ry, ry + rh):
            for x in range(rx, rx + rw):
                if x >= W or y >= H:
                    continue
                if y == ry or y == ry + rh - 1 or x == rx or x == rx + rw - 1:
                    s[y][x] = 1
                    g[y][x] = T_STONE
                else:
                    g[y][x] = T_SAND
                    s[y][x] = 0
        door_x = rx + rw // 2
        door_y = min(ry + rh - 1, 23)
        g[door_y][door_x] = T_SAND
        s[door_y][door_x] = 0

    m["ents"] = [
        ("npc_elder",     12, 12),
        ("enemy_cultist", 30, 10),
        ("enemy_cultist", 12, 37),
        ("enemy_knight",  50, 14),
        ("enemy_knight",  52, 36),
        ("boss_blight",   57, 23),
        ("portal",         2, 24, "pass", 60, 24),
    ]

    # 墓碑群
    for (gx, gy, gw, gh) in [(9, 7, 12, 10), (9, 31, 10, 12), (44, 8, 12, 10)]:
        for _ in range(24):
            px = gx + rnd.randrange(1, gw - 1)
            py = gy + rnd.randrange(1, gh - 1)
            if free(m, px, py):
                put(m, px, py, rnd.choice([P_GRAVE1, P_GRAVE2, P_GRAVE3]))

    scatter(m, rnd, P_SKULL, 14)
    scatter(m, rnd, P_BONE, 24)
    scatter(m, rnd, [P_JAR1, P_JAR2], 16)
    scatter(m, rnd, [P_BARREL, P_CRATE2], 10)

    put(m, 30, 17, P_PIANO)
    put(m, 44, 40, P_LADDER)
    put(m, 18, 40, P_CHEST)

    return dict(name="crypt", tileset="terrain_summer.png",
                bg=None, weather="none", m=m)


# ---------------------------------------------------------------
# 5) 远古兽巢
# ---------------------------------------------------------------
def gen_nest():
    m = new_map()
    g, s, d = m["ground"], m["solid"], m["decor"]
    rnd = random.Random(5)

    for y in range(H):
        for x in range(W):
            n = noise2(x, y, 13) * 0.65 + noise2(x, y, 31, scale=2.0) * 0.35
            g[y][x] = T_SAND if n > 0.66 else (T_GROUND_ALT if n > 0.44 else T_GROUND)

    m["ents"] = [
        ("enemy_lizard",    16, 18),
        ("enemy_lizard",    22, 34),
        ("enemy_insect",    30, 14),
        ("enemy_insect",    36, 30),
        ("enemy_insect",    26, 40),
        ("enemy_plant",     18, 28),
        ("enemy_plant",     44, 20),
        ("enemy_turtle",    40, 38),
        ("enemy_mini_tyranno", 48, 14),
        ("enemy_mini_tyranno", 52, 34),
        ("enemy_ptero",     34, 22),
        ("enemy_ptero",     46, 26),
        ("boss_trex",       55, 24),
        ("portal",           2, 24, "crypt", 60, 24),
    ]

    scatter(m, rnd, P_TREE2, 12)
    scatter(m, rnd, P_TREE1, 22)
    scatter(m, rnd, [P_CACTUS1, P_CACTUS2, P_CACTUS3], 40)
    scatter(m, rnd, [P_ROCK4, P_ROCK5], 24)
    scatter(m, rnd, P_BONE, 30)
    scatter(m, rnd, P_SKULL, 10)
    scatter(m, rnd, P_BUSH, 26)

    # 兽巢中心：一圈骨头
    for ang in range(0, 360, 30):
        rad = math.radians(ang)
        px = 34 + int(6 * math.cos(rad))
        py = 24 + int(4 * math.sin(rad))
        if free(m, px, py):
            put(m, px, py, P_BONE)

    return dict(name="nest", tileset="terrain_spring.png",
                bg=None, weather="fog", m=m)


# ---------------------------------------------------------------
# 6) 星陨裂隙
# ---------------------------------------------------------------
def gen_rift():
    m = new_map()
    g, s, d = m["ground"], m["solid"], m["decor"]
    rnd = random.Random(6)

    for y in range(H):
        for x in range(W):
            g[y][x] = T_RIFT_VOID
            s[y][x] = 1

    islands = [(16, 16, 9, 5), (32, 26, 11, 6), (48, 16, 9, 5), (24, 38, 8, 4), (46, 38, 8, 4)]
    for (cx, cy, rx, ry) in islands:
        for y in range(cy - ry, cy + ry + 1):
            for x in range(cx - rx, cx + rx + 1):
                if x < 0 or y < 0 or x >= W or y >= H:
                    continue
                if ((x - cx) / rx) ** 2 + ((y - cy) / ry) ** 2 <= 1.0:
                    g[y][x] = T_RIFT_PLATFORM
                    s[y][x] = 0

    # 桥
    bridges = [(25, 16, 21, 26), (43, 26, 39, 16), (32, 32, 24, 38), (43, 26, 46, 38)]
    for (x0, y0, x1, y1) in bridges:
        steps = max(abs(x1 - x0), abs(y1 - y0))
        for i in range(steps + 1):
            x = x0 + (x1 - x0) * i // max(1, steps)
            y = y0 + (y1 - y0) * i // max(1, steps)
            for dy in (0, 1):
                if 0 <= y + dy < H:
                    g[y + dy][x] = T_RIFT_PLATFORM
                    s[y + dy][x] = 0

    m["ents"] = [
        ("enemy_ship",  20, 14),
        ("enemy_ship",  34, 24),
        ("enemy_ship",  46, 14),
        ("enemy_ship",  50, 40),
        ("enemy_insect", 28, 40),
        ("enemy_insect", 38, 18),
        ("enemy_insect", 42, 30),
        ("boss_dragon", 52, 26),
        ("portal",       3, 24, "nest", 60, 24),
    ]

    scatter(m, rnd, P_METEOR, 22)
    scatter(m, rnd, [P_ROCK4, P_ROCK5, P_ROCK6], 30)
    scatter(m, rnd, P_PLANET, 3)
    scatter(m, rnd, P_CHEST, 2)
    scatter(m, rnd, P_SIGN1, 1)

    return dict(name="rift", tileset="0-tileset-32x32.png",
                bg=None, weather="meteor", m=m)


# ---------------------------------------------------------------
def write_map(spec):
    m = spec["m"]
    bg = spec.get("bg")
    lines = [
        "# SIMDRPG map v1",
        f"name    {spec['name']}",
        f"tileset {spec['tileset']}",
        f"bg      {bg if bg else 'none'}",
        f"size    {W} {H}",
        f"weather {spec['weather']}",
        "[GROUND]",
    ]
    for row in m["ground"]:
        lines.append(" ".join(str(v) for v in row))
    lines.append("[SOLID]")
    for row in m["solid"]:
        lines.append(" ".join(str(v) for v in row))
    lines.append("[DECOR]")
    for row in m["decor"]:
        lines.append(" ".join(str(v) for v in row))
    lines.append("[ENTITY]")
    for e in m["ents"]:
        lines.append(" ".join(str(v) for v in e))

    path = os.path.join(OUT, f"{spec['name']}.map")
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("\n".join(lines) + "\n")

    solid_n = sum(sum(r) for r in m["solid"])
    decor_n = sum(1 for row in m["decor"] for v in row if v)
    kinds = len({v for row in m["decor"] for v in row if v})
    print(f"  {spec['name']:8s} {W}x{H}  {spec['tileset']:24s} "
          f"天气={spec['weather']:6s} 实体{len(m['ents']):2d} 障碍{solid_n:5d} "
          f"装饰{decor_n:4d}({kinds:2d}种)")


if __name__ == "__main__":
    os.makedirs(OUT, exist_ok=True)
    print(f"生成地图 -> {OUT}")
    for gen in (gen_village, gen_forest, gen_pass, gen_crypt, gen_nest, gen_rift):
        write_map(gen())
