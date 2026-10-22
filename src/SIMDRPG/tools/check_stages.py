#!/usr/bin/env python3
"""逐阶段增量编译检查。

模拟学员按讲义顺序做：每完成一个阶段，把"到目前为止建好的文件"
一起编译（只 /c，不链接），必须 0 错误。

如果某个阶段编不过，说明讲义的文件顺序或 include 有问题——
学员会在那里卡住。

用法：
    python tools/check_stages.py [cl.exe 的路径]
环境变量：
    MSVC_INC / SDK_INC / SDL_INC / SDLIMAGE_INC   可选，覆盖默认路径
"""
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "verify", "full")

DEFAULT_CL = (r"D:/Program Files (x86)/Microsoft Visual Studio/2019/Community"
              r"/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
VS_ROOT = r"D:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133"
WIN_KIT = r"C:/Program Files (x86)/Windows Kits/10"
SDK_VER = "10.0.19041.0"

# 阶段 -> 该阶段结束时应该存在的源文件（累积）
STAGES = [
    (0,  ["src/platform/Window.cpp"]),
    (1,  ["include/core/Types.h", "include/gfx/Surface.h", "src/gfx/Surface.cpp"]),
    (2,  ["include/gfx/Atlas.h", "src/gfx/Atlas.cpp"]),
    (3,  ["include/gfx/Blitter.h", "src/gfx/Blitter.cpp"]),
    (4,  ["include/simd/Backend.h", "src/simd/Kernels.cpp",
          "src/simd/KernelsAVX2.cpp", "src/simd/Backend.cpp"]),
    (5,  ["include/world/TileMap.h", "src/world/TileMap.cpp"]),
    (6,  ["include/world/Camera.h"]),
    (7,  ["include/game/Animator.h"]),
    (8,  ["include/world/Collision.h", "src/world/Collision.cpp",
          "include/platform/Input.h", "src/platform/Input.cpp"]),
    (9,  ["include/ecs/Registry.h", "include/game/Components.h"]),
    (10, ["include/game/Combat.h", "include/game/Player.h", "include/game/Inventory.h"]),
    (11, ["include/game/Enemy.h", "src/game/Enemy.cpp"]),
    (12, ["include/game/Dialog.h", "include/game/Quest.h"]),
    (13, ["include/world/Weather.h", "src/world/Weather.cpp"]),
    (15, ["include/gfx/Ui.h", "src/gfx/Ui.cpp"]),
    (17, ["include/game/Game.h", "src/game/Game.cpp", "src/main.cpp"]),
]

# 阶段 3 的 Blitter.cpp 是"过渡版"：只做标量混合，不依赖 simd（阶段 4 才接入）
TRANSITION_STAGE = 3


def stage3_blitter(text: str) -> str:
    text = text.replace('#include "simd/Backend.h"\n', "")
    text = text.replace(
        "        simd::blend_row(d, s, w);          // ← 阶段 4 换的就是这一行",
        "        for (int x = 0; x < w; ++x) blend_pixel_scalar(d[x], s[x]);")
    return text


def includes():
    return [
        f'/I{SRC}/include',
        f'/I{os.environ.get("SDL_INC", os.path.join(ROOT, "..", "..", "Thirdparty/SDL2/SDL-release-2.32.6/include"))}',
        f'/I{os.environ.get("SDLIMAGE_INC", os.path.join(ROOT, "..", "..", "Thirdparty/SDL2/SDL_image-release-2.8.0/include"))}',
        f'/I{os.environ.get("MSVC_INC", VS_ROOT + "/include")}',
        f'/I{os.environ.get("SDK_INC", WIN_KIT + "/Include/" + SDK_VER)}/ucrt',
        f'/I{os.environ.get("SDK_INC", WIN_KIT + "/Include/" + SDK_VER)}/um',
        f'/I{os.environ.get("SDK_INC", WIN_KIT + "/Include/" + SDK_VER)}/shared',
    ]


def main():
    cl = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_CL
    if not os.path.exists(cl):
        print("找不到 cl.exe:", cl)
        return 2

    # 临时目录放在仓库内（系统 %TEMP% 可能被沙箱拦）。
    # 不在这里 rmtree：批量删除会触发沙箱确认；obj 复用同名覆盖，
    # 跑完手动删 verify/full/_stagecheck 即可。
    tmp = os.path.join(ROOT, "verify", "full", "_stagecheck")
    os.makedirs(tmp, exist_ok=True)
    have = []
    failures = 0

    try:
        for stage, new_files in STAGES:
            have.extend(new_files)
            cpps = [f for f in have if f.endswith(".cpp")]

            # 准备源文件（阶段 3 用过渡版）
            work = os.path.join(tmp, f"s{stage}")
            os.makedirs(work)
            args = []
            for rel in cpps:
                src_path = os.path.join(SRC, rel)
                dst_rel = rel
                if stage == TRANSITION_STAGE and rel == "src/gfx/Blitter.cpp":
                    dst_path = os.path.join(work, "Blitter.cpp")
                    open(dst_path, "w", encoding="utf-8").write(
                        stage3_blitter(open(src_path, encoding="utf-8").read()))
                    args.append(dst_path)
                else:
                    args.append(src_path)

            cmd = [cl, "/nologo", "/utf-8", "/std:c++20", "/DSDL_MAIN_HANDLED",
                   "/EHsc", "/c"] + includes() + args + [f"/Fo{work}/"]

            r = subprocess.run(cmd, capture_output=True, text=True,
                               encoding="utf-8", errors="replace")
            errs = [l for l in (r.stdout + r.stderr).split("\n") if "error" in l]
            status = "OK  " if r.returncode == 0 else "FAIL"
            print(f"  阶段 {stage:2d}: {status}  {len(cpps)} 个 .cpp")
            if r.returncode != 0:
                failures += 1
                for l in errs[:6]:
                    print("        ", l.strip()[:150])
    finally:
        # 故意不删 tmp（下面这些 obj 是几十上百个文件，批量删会被沙箱拦）。
        # 想清就手动删 verify/full/_stagecheck。
        pass

    print()
    if failures:
        print(f"有 {failures} 个阶段编译不过——学员会卡在那里。")
        return 1
    print("所有阶段按顺序都能编译通过。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
