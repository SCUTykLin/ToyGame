#!/bin/bash
# 编译参考实现（verify/full）成 simdrpg.exe。
# 用法：bash tools/build_ref.sh [输出名]
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/verify/full" || exit 1

OUT="${1:-simdrpg.exe}"
mkdir -p _obj

CL="D:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe"
V="D:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133"
WK="C:/Program Files (x86)/Windows Kits/10"
SDLI="E:/toy-projects/Thirdparty/SDL2/SDL-release-2.32.6/include"
IMG_I="E:/toy-projects/Thirdparty/SDL2/SDL_image-release-2.8.0/include"
IMGUI="E:/toy-projects/Thirdparty/imgui"
LIB="E:/toy-projects/cmake-build-debug/thirdparty/SDL2"

export MSYS_NO_PATHCONV=1
"$CL" /nologo /utf-8 /wd4819 /std:c++20 /MDd /DSDL_MAIN_HANDLED /EHsc \
  /Iinclude /I"$SDLI" /I"$IMG_I" /I"$IMGUI" /I"$IMGUI/backends" \
  /I"$V/include" /I"$WK/Include/10.0.19041.0/ucrt" \
  /I"$WK/Include/10.0.19041.0/um" /I"$WK/Include/10.0.19041.0/shared" \
  src/*.cpp src/platform/*.cpp src/gfx/*.cpp src/simd/*.cpp \
  src/world/*.cpp src/game/*.cpp src/ui/*.cpp \
  "$IMGUI/imgui.cpp" "$IMGUI/imgui_draw.cpp" "$IMGUI/imgui_tables.cpp" \
  "$IMGUI/imgui_widgets.cpp" \
  "$IMGUI/backends/imgui_impl_sdl2.cpp" \
  "$IMGUI/backends/imgui_impl_sdlrenderer2.cpp" \
  /Fe:"$OUT" /Fo_obj/ \
  /link \
  /LIBPATH:"$V/lib/x64" \
  /LIBPATH:"$WK/Lib/10.0.19041.0/um/x64" \
  /LIBPATH:"$WK/Lib/10.0.19041.0/ucrt/x64" \
  "$LIB/SDL-release-2.32.6/SDL2-staticd.lib" \
  "$LIB/SDL_image-release-2.8.0/SDL2_image-staticd.lib" \
  winmm.lib imm32.lib ole32.lib oleaut32.lib version.lib uuid.lib \
  advapi32.lib setupapi.lib shell32.lib user32.lib gdi32.lib \
  > _build.log 2>&1
rc=$?
if [ $rc -ne 0 ]; then
  echo "BUILD FAILED ($rc)"
  "C:/Users/YangKai.Lin/.workbuddy/binaries/python/envs/default/Scripts/python.exe" - <<'PY'
import re
d = open('_build.log', 'rb').read().decode('utf-8', 'replace')
d = re.sub(r'[^\x20-\x7e\n]', '', d)
for l in [x for x in d.split('\n') if 'error' in x][:12]:
    print(' ', l[:170])
PY
  exit 1
fi
echo "BUILD OK -> verify/full/$OUT"
