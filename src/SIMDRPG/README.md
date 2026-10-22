# 《Eldra 之烬》 — C++20 / SDL2 / SIMD 手写 2D RPG

一个从零手写的俯视角动作 RPG，用来练 **C++20 + 软件渲染 + SIMD + ECS**。
地块 **32×32**，角色 **64×64**，四季地形图集，**1194 个素材全部能在游戏内浏览/使用**
（`F2` 打开编辑器就能翻），约 560 张已经接进渲染。

**讲义是唯一事实来源**：`verify/full/` 里的 49 个源文件由讲义中的代码块自动生成
（`tools/extract_code.py`），覆盖率审计为 **100%**。
你按讲义抄一遍，得到的就是这个能编译、能跑、能通关的工程。

---

## 快速开始：三条路，选一条

### A. 我想跟着抄（推荐）

```bash
# 1. 先看要建哪些文件、什么顺序
#    docs/03f-完整文件附录.md 第一节有 9 批建文件顺序

# 2. 从阶段 0 开始，按 03a → 03b → 03c → 03d → 03e → 03g 抄
#    每个文件都是完整代码块，整块粘进同名文件

# 3. 构建（每建完一批就跑一次，错误不会超过 3 条）
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j

# 4. 运行（必须在仓库根目录，否则找不到 assets/）
./build/bin/simdrpg
```

### B. 我只想先看它跑起来

```bash
cd verify/full

# 用 SDL 源码构建（首次约 3–5 分钟），或先装好 SDL2 再 -DSDL2_DIR=...
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j

cd ../..                    # 回到仓库根目录
./verify/full/build/bin/simdrpg
```

### C. 直接用编译好的参考实现

仓库里带了一个已经编译好的 `verify/full/simdrpg_reference.exe`（2MB，静态链接 SDL）：

```bash
./verify/full/simdrpg_reference.exe
```

**必须在仓库根目录运行**（它按相对路径找 `assets/`）。
这个 exe 对应的是 `verify/full/` 里的代码，不是你的 `src/`。
改完代码想跑自己的版本，用 A 或 B 的方式构建。

### D. 无显示器环境验证（CI / 远程终端）

```bash
./verify/full/simdrpg_reference.exe --selftest 300
```

输出：

```
[simd] backend = AVX2
[game] 加载道具池...
[game] 加载特效池...
[game] 道具 440 个, 特效 85 个
[game] 载入地图 village
[map] village  64x48  tpr=16  entities=4
[game] 退出（跑了 300 帧）
```

**这条命令跑通了，说明整个游戏循环（加载 → 更新 → 渲染 → 退出）没有崩溃。**

### E. 截图

游戏内置截图模式（无头渲染，存 PNG）：

```bash
./verify/full/simdrpg_reference.exe --shot <地图> <x> <y> <帧数> <输出.png>

# 例：跳到裂隙第 40 格、跑 150 帧、存图
./verify/full/simdrpg_reference.exe --shot rift 40 24 150 shots/06_rift.png
```

`shots/` 里已经有七张：村庄路口、森林（雨）、山道（雪）、地窖、兽巢（雾）、裂隙（流星）、Boss 房。

---

## 操作

| 键 | 作用 |
|----|------|
| `WASD` / 方向键 | 移动 |
| `J` / 空格 | 攻击 |
| `E` | 交互（对话、传送门） |
| `Q` | 切换武器 |
| `F1` | 进 / 出**手搓版**地图编辑器 |
| `F2` | 进 / 出 **ImGui 实时编辑器**（改完存盘，热加载回游戏） |
| `ESC` | 退出 |

### 场景里能做什么

| 物件 | 操作 | 结果 |
|------|------|------|
| 宝箱 `chest` | 走近按 `E` | +25 金币、掉 3 个道具、箱子消失 |
| 路牌 `sign` | 走近按 `E` | 显示该地图的碑文（每张图不同） |
| 篝火 `fire` | 走近按 `E` | 回 30 HP（满血时提示） |
| 陶罐 `jar` | 攻击 | 打碎，掉道具 |
| NPC | 走近按 `E` | 对话（打字机效果 + 九宫格对话框） |

靠近可交互物时屏幕底部会出现 `[E] Open chest` / `[E] Read` / `[E] Rest` 提示。

**对话是英文的**：`gfx::draw_text` 只支持 ASCII，汉字会被整个跳过——
写成中文的话对话框会一片空白（我踩过这个坑，还以为是对话没触发）。

---

## 实时编辑器（F2，ImGui）

游戏跑着的时候按 `F2`，弹出 ImGui 做的编辑器；改完点 `Save`，
再点 `Reload map`，世界立刻变成新的 —— **不重启、不重编译、玩家位置和血量都保住**。

![编辑器](shots/10_ui_editor.png)

| 键 / 操作 | 作用 |
|-----------|------|
| `F2` | 开 / 关（游戏暂停，相机还能挪） |
| `1` `2` `3` | GROUND / SOLID / DECOR 图层 |
| 左键 / 右键 / 中键 | 画 / 擦 / 吸管 |
| `[` `]` | 画笔 1 → 3 → 5 |
| `Ctrl+Z` | 撤销一笔 |
| `F5` | 保存 `.map` + `assets/props.cfg` |
| `Reload map` | 从磁盘重读地图，**玩家原地不动、血不变** |
| 右侧搜索 + 分类 | 在 1194 张素材里找图，点 `register as prop` 就能往地图上放 |

**热重载贴图**：游戏开着，用画图改一张 `assets/gfx/backgrounds/tree-1.png` 并保存，
0.4 秒内游戏里那棵树就变了（控制台打 `[assets] 热重载 1 个文件`）。

实现、踩坑、逐段讲解全在 **`docs/03g-实时编辑器与热重载.md`**。

编辑器按 01 章的分层拆成三层，不是一坨：

| 层 | 文件 | 职责 |
|----|------|------|
| `game/` | `Game` | 装配、F2 开关、暂停、热加载地图 |
| `ui/` | `ui::EditorUi` | 只画 ImGui 面板，只发"意图" |
| `world/` | `world::EditSession` | 只改地图/道具、撤销、保存，**不认识 ImGui** |

所以 `tests/session_test.cpp` 不开窗口就能测"画一笔、撤销、落笔刷碰撞、存盘"。

```bash
# 不进游戏也能截编辑器（无头，连 UI 一起截）
./verify/full/simdrpg_reference.exe --ui-shot village 32 20 12 shots/10_ui_editor.png
```

---

## 地图编辑器（F1，手搓版）

游戏里按 `F1` 就是编辑器，不用另开程序。改完按 `F5` 存回当前地图（例如 `assets/maps/village.map`）。

| 键 | 作用 |
|----|------|
| `F1` | 进 / 出编辑模式 |
| `WASD` / 方向键 | 平移相机（编辑时相机不跟玩家） |
| 左键拖动 | 画当前 tile |
| 右键 | 擦（地形擦回 17，碰撞擦成 0） |
| 中键 | 吸管，把光标下的 tile 捡起来当画笔 |
| 滚轮 / `Q` `E` | 换画笔 |
| 点左上角调色板 | 直接选 tile |
| `Tab` | 换图集（四季 + ninja，换完自动修正越界索引） |
| `T` | 切换 地形 / 碰撞 模式（碰撞层叠 20% 红显示） |
| `F5` | 保存 |

底部状态栏：`地图名  图集名  tile 12/415  GROUND  *`，`*` 表示有未保存改动。

```bash
# 不进游戏也能截编辑器界面（无头）
./verify/full/simdrpg_reference.exe --edit-shot village 26 22 5 shots/editor.png
```

编辑器分两半：`TileMap` 上的 `paint / set_solid / save / set_tileset` 是**纯逻辑**（不碰 SDL），
`Game::editor_update / editor_draw` 只是外壳。所以它能被脚本测——
`verify/full/tests/edit_test.cpp` 会画 500 笔、存盘、重读、逐格比对。

---

## 六张地图

| # | 地图 | tileset | 天气 | Boss |
|---|------|---------|------|------|
| 1 | 晨曦村 | `terrain_summer` | 晴 | —（3 个 NPC） |
| 2 | 低语森林 | `terrain_autumn` | 雨 | —（门禁：需要剑） |
| 3 | 霜噬山道 | `terrain_winter` | 雪 | —（门禁：森之核） |
| 4 | 灰烬地窖 | `terrain_summer` | 无 | `boss_blight` → 灰烬钥匙 |
| 5 | 远古兽巢 | `terrain_spring` | 雾 | `boss_trex` → 原始之血 |
| 6 | 星陨裂隙 | `0-tileset-32x32`（在 `backgrounds/`） | 流星雨 | `boss_dragon` → 结局 |

剧情是「世界之树 Eldra 被虚蚀侵蚀，集四件圣物封印」。
**靠任务旗标卡通路，不靠刷怪**：没拿到对应圣物，传送门会告诉你缺什么。

---

## 文档

| 文档 | 内容 | 行数 |
|------|------|------|
| `docs/00-世界观与六张地图.md` | 设定、地图、28 种敌人表、旗标、结局 | 222 |
| `docs/01-架构设计.md` | 七层分层、接口契约 | 372 |
| `docs/02-SIMD从零到能用.md` | **SIMD 主战场**：寄存器模型、div255 推导、SSE2 逐行注释、AVX2 的坑 | 996 |
| `docs/03-实战分步实现.md` | 索引（指向 03a–03g） | 100 |
| `docs/03a-渲染基础.md` | 阶段 0–4：窗口 / Surface / 读图 / Blitter / SIMD | 1664 |
| `docs/03b-世界与角色.md` | 阶段 5–9：地图 / 相机 / 动画 / 碰撞 / 输入 / ECS | 1175 |
| `docs/03c-战斗与AI.md` | 阶段 10–11：近战 / 投射物 / 武器 / 28 行敌人表 | 851 |
| `docs/03d-内容与表现.md` | 阶段 12–16：对话 / 任务 / 天气 / 视差 / UI / Boss | 1063 |
| `docs/03e-系统整合.md` | 阶段 17–23：Game 类 / 主循环 / **完整 Game.cpp** / **地图编辑器** | 2205 |
| `docs/03f-完整文件附录.md` | 文件索引、建文件顺序、排错顺序 | 300 |
| `docs/03g-实时编辑器与热重载.md` | **阶段 24：ImGui 编辑器 / 素材库 / 热加载** | 3532 |
| `docs/04-性能与调试.md` | 怎么测才不是自欺欺人 | 325 |
| `docs/05-素材分配表.md` | 1194 个素材各自的去处 | 344 |

**读的顺序**：`00` → `01` → `03a` 开始抄 → 卡住了查 `03f`。
`02` 在做到阶段 4 之前读一遍。

---

## 仓库结构

```
├── docs/                     ← 讲义（唯一事实来源）
├── assets/
│   ├── gfx/                  ← 1194 张素材 + 58 张预览/拼图
│   └── maps/                 ← 6 张地图
├── tools/
│   ├── build_sprites.py      ← LPC 部件 → 角色 spritesheet
│   ├── gen_maps.py           ← 程序化生成 6 张地图
│   ├── make_contact_sheet.py ← 素材拼图
│   ├── extract_code.py       ← ★ 从讲义提取完整文件到 verify/full
│   ├── audit_coverage.py     ← ★ 审计"讲义 vs 实现"的覆盖率
│   ├── sync_docs.py          ← 改完源码后把代码块同步回讲义
│   └── verify_simd.cpp       ← SIMD 正确性验证器（独立单文件）
├── verify/full/              ← 参考实现（由讲义生成，不手改）
│   ├── include/ui/           ← 界面层（EditorUi）
│   └── include/world/EditSession.h  ← 编辑逻辑（无 ImGui 依赖）
├── src/                      ← 你的代码（现在是空的）
└── CMakeLists.txt            ← 也由讲义生成
```

**`verify/full/` 里的文件不要手改**——它们是 `tools/extract_code.py` 生成的，
下次运行会被覆盖。要改代码，改 `docs/` 里的代码块。

---

## 素材

主源是 **LPC Revised**（`github.com/ElizaWy/LPC`，OGA-BY 3.0），
加上 7 个 superpowers 素材包（CC0）。

| 目录 | 数量 | 说明 |
|------|------|------|
| `objects/` | 440 | 道具（递归含子目录） |
| `backgrounds/` | 162 | 远景 |
| `monsters/` | 50 | 怪物 |
| `structures/` | 120 | 建筑 |
| `ui/` | 117 | 血条、心、对话框、立绘 |
| `fx/` | 85 | 爆炸、斩击、水波 |
| `chars/` | 190 | 7 个合成角色 + 异界角色 |
| `terrain/` | 22 | 四季地形图集 |
| `fonts/` | 9 | 点阵字体 |

**署名是发布时的硬要求**，见 `assets/CREDITS.md`。

---

## 工具

```bash
# 重新生成角色（改 CHARACTERS 字典里的配色后跑）
python tools/build_sprites.py

# 重新生成六张地图（改顶部 tile 索引常量后跑）
python tools/gen_maps.py

# 看素材（1194 个文件，先看拼图）
python tools/make_contact_sheet.py

# 改完讲义后，重新生成参考实现
python tools/extract_code.py

# 审计讲义覆盖率（必须是 100%）
python tools/audit_coverage.py
```

---

## 已知取舍（不是 bug）

1. **中文不显示**。`fonts/*.png` 只有 ASCII 32–126。对话是中文，会渲染成空白。
   方案见 `03d` 15.6：先全英文跑通，最后接 SDL_ttf。
2. **没有音效**。`03e` 末尾给了接入方式。
3. **没有存档**。`03e` 末尾给了 `Save` 结构体的写法。
4. **战斗动画是程序化的**。LPC 的战斗 spritesheet 有 60/112 帧是空的，
   所以攻击变成"0.12 秒的判定框 + 特效"，不依赖动画帧。
5. **Boss 不换贴图**，只改数值（速度 / 冷却 / 震屏）。

---

## 验收清单

- [ ] `python tools/audit_coverage.py` 输出 `总覆盖率 100.0%`
- [ ] `cd verify/full && cmake -B build && cmake --build build` 成功
- [ ] `./verify/full/simdrpg_reference.exe --selftest 300` 无崩溃
- [ ] 双击运行，按 WASD 能走，按 J 能打
- [ ] 和长老说话，拿到森之核，村口传送门从"过不去"变成能进
- [ ] 按 `F2` 弹出编辑器，画两笔、`F5` 保存、`Reload map` 后玩家还在原地
- [ ] 改一张 PNG，0.4 秒内游戏里跟着变
- [ ] `asset_test.exe` 输出"全部通过（失败 0 项）"
