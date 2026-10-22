# 素材来源与许可

本项目所有美术素材均来自自由许可的公开素材库，**不是 AI 生成**，
可以用于学习、修改，也可以用于商业项目（需遵守下列各自的署名要求）。

---

## 1. LPC Revised（主要素材）

- **来源**：https://github.com/ElizaWy/LPC
- **许可**：OGA-BY 3.0（基于 CC-BY 3.0，去掉了 DRM 限制条款）
- **要求**：必须署名作者，可商用、可修改、可再分发
- **分辨率**：32×32 的地块（tile），角色 64×64 每帧
- **风格**：三人称俯视（three-quarters top-down），~170 色统一调色板

### 用到的部分

| 目录 | 内容 | 用在哪 |
|------|------|--------|
| `Terrain/terrain_{spring,summer,autumn,winter,winter_ice}.png` | 四季地形图集，512×832 = 16×26 个 32×32 地块 | 四张地图的地面层 |
| `Terrain/trees_*.png`、`plants_*.png` | 树木、灌木 | 森林地图 |
| `Terrain/flowers.png`、`mushrooms.png`、`ice-shallows.png` | 装饰 | 各地图点缀 |
| `Characters/Body/`、`Head/`、`Hair/`、`Clothing/`、`Head Accessories/`、`Props/` | 模块化角色部件 | 合成 7 个角色 spritesheet |
| `Objects/` | 箱子、木桶、篝火、矿石、地牢装饰 | 场景道具 |
| `FX/` | 水花、水波纹 | 天气与水面特效 |

### 必须署名的作者

```
Eliza Wyatt (DeathsDarling)
Lanea Zimmerman (Sharm)
Stephen Challener (Redshrike)
Johannes Sjölund (Wulax)
BlueCarrot16
BenCreating
Durrani
YuriNikolai
Craftpix.net 2D Game Assets
```

> 游戏里的 credits 界面请把这些名字列出来。

---

## 2. Superpowers Asset Packs（备用，本项目未直接使用）

- **来源**：https://github.com/sparklinlabs/superpowers-asset-packs
- **许可**：CC0 1.0（公共领域，无需署名）
- **说明**：`medieval-fantasy` 包里有角色、怪物、道具、HUD、字体，
  但分辨率是 16px 级别，达不到本项目「比 RolePlayingGame 更精细」的要求，
  所以只作为备选保留。如果你想要像素更小、更复古的风格，可以换用它。

---

## 3. 角色是怎么来的

LPC 的角色是**模块化**的：身体、头、头发、上衣、裤子、鞋、头盔、武器
分别存成独立的 spritesheet，需要按图层顺序合成。

本项目的 7 个角色由 `tools/build_sprites.py` 自动合成：

| 名字 | 身份 | 外观 |
|------|------|------|
| `hero` | 主角 Aric | 蓝紫长袖、棕发、皮靴 |
| `elder` | 长老 Maelor | 白头白须、紫袍 |
| `smith` | 铁匠 Brann | 黑发黑须、棕衣、工装 |
| `merchant` | 商人 Sela | 金发、绿衣 |
| `bandit` | 强盗 | 眼罩、炭灰衣 |
| `cultist` | 虚蚀教徒 | 锁甲头罩、紫袍 |
| `knight` | 黑骑士 | 全封闭头盔、深色甲 |

想改外观：编辑 `build_sprites.py` 里的 `CHARACTERS` 字典，
改肤色 / 发色 / 衣服配色，重跑脚本即可。
LPC 每种衣服有 30+ 种配色（Aegean、Amber、Amethest、Black、Bronze……）。
