#!/bin/bash
# 编译 verify/full/tests 下的测试程序（不需要窗口）。
# 用法：bash tools/build_test.sh [asset|session|content|edit]
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/verify/full" || exit 1

WHICH="${1:-asset}"
case "$WHICH" in
  asset)   SRC="tests/asset_test.cpp src/world/AssetBank.cpp src/world/Props.cpp src/gfx/Atlas.cpp src/gfx/Surface.cpp"; OUT="asset_test.exe" ;;
  session) SRC="tests/session_test.cpp src/world/EditSession.cpp src/world/TileMap.cpp src/world/AssetBank.cpp src/world/Props.cpp src/gfx/Atlas.cpp src/gfx/Surface.cpp"; OUT="session_test.exe" ;;
  content) SRC="tests/content_check.cpp src/world/TileMap.cpp src/game/Enemy.cpp src/gfx/Atlas.cpp src/gfx/Surface.cpp"; OUT="content_check.exe" ;;
  edit)    SRC="tests/edit_test.cpp src/world/TileMap.cpp src/gfx/Atlas.cpp src/gfx/Surface.cpp"; OUT="edit_test.exe" ;;
  *) echo "用法: $0 [asset|session|content|edit]"; exit 1 ;;
esac

CL="D:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe"
V="D:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133"
WK="C:/Program Files (x86)/Windows Kits/10"
SDLI="E:/toy-projects/Thirdparty/SDL2/SDL-release-2.32.6/include"
IMG_I="E:/toy-projects/Thirdparty/SDL2/SDL_image-release-2.8.0/include"
LIB="E:/toy-projects/cmake-build-debug/thirdparty/SDL2"

export MSYS_NO_PATHCONV=1
"$CL" /nologo /utf-8 /wd4819 /std:c++20 /MDd /DSDL_MAIN_HANDLED /EHsc \
  /Iinclude /I"$SDLI" /I"$IMG_I" \
  /I"$V/include" /I"$WK/Include/10.0.19041.0/ucrt" \
  /I"$WK/Include/10.0.19041.0/um" /I"$WK/Include/10.0.19041.0/shared" \
  $SRC /Fe:"$OUT" /Fo_obj/ \
  /link /LIBPATH:"$V/lib/x64" /LIBPATH:"$WK/Lib/10.0.19041.0/um/x64" /LIBPATH:"$WK/Lib/10.0.19041.0/ucrt/x64" \
  "$LIB/SDL-release-2.32.6/SDL2-staticd.lib" "$LIB/SDL_image-release-2.8.0/SDL2_image-staticd.lib" \
  winmm.lib imm32.lib ole32.lib oleaut32.lib version.lib uuid.lib \
  advapi32.lib setupapi.lib shell32.lib user32.lib gdi32.lib > _test.log 2>&1
rc=$?
if [ $rc -ne 0 ]; then
  echo "BUILD FAILED ($rc)"
  grep -i "error" _test.log | head -10
  exit 1
fi
echo "BUILD OK -> verify/full/$OUT"
