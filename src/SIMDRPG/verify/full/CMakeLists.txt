# 参考实现的构建脚本。
#
# 这个文件**不在讲义里**——它只是给 verify/full/ 用的。
# 你自己写代码时用的是仓库根目录的 CMakeLists.txt（那个是讲义里的）。
#
# 用法：
#   cd verify/full
#   cmake -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build --config Release -j
#   cd ../.. && ./verify/full/build/Release/simdrpg     # 必须在仓库根目录运行

cmake_minimum_required(VERSION 3.20)
project(simdrpg_reference CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

if(MSVC)
    add_compile_options(/utf-8 /W3)
    add_compile_definitions(SDL_MAIN_HANDLED)
endif()

# Thirdparty 可能在项目里，也可能在项目外，向上找最多 5 层
function(_find_tp out_var rel)
    set(_probe "${CMAKE_CURRENT_SOURCE_DIR}")
    set(${out_var} "" PARENT_SCOPE)
    foreach(_i RANGE 5)
        if(EXISTS "${_probe}/${rel}/CMakeLists.txt")
            set(${out_var} "${_probe}/${rel}" PARENT_SCOPE)
            return()
        endif()
        get_filename_component(_probe "${_probe}" DIRECTORY)
    endforeach()
endfunction()

_find_tp(_SDL2 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Thirdparty/SDL2/SDL-release-2.32.6")
_find_tp(_SDL2B "Thirdparty/SDL2/SDL-release-2.32.6")
if(NOT _SDL2)
    set(_SDL2 "${_SDL2B}")
endif()
set(SDL2_ROOT "${_SDL2}" CACHE PATH "")

if(NOT EXISTS "${SDL2_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "找不到 SDL2 源码: ${SDL2_ROOT}")
endif()

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)
add_subdirectory("${SDL2_ROOT}" "${CMAKE_BINARY_DIR}/_sdl2" EXCLUDE_FROM_ALL)

_find_tp(_SDL2IMG "Thirdparty/SDL2/SDL_image-release-2.8.0")
set(SDL2_IMAGE_ROOT "${_SDL2IMG}" CACHE PATH "")
if(NOT EXISTS "${SDL2_IMAGE_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "找不到 SDL2_image 源码: ${SDL2_IMAGE_ROOT}")
endif()

set(SDL2IMAGE_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_STATIC ON  CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory("${SDL2_IMAGE_ROOT}" "${CMAKE_BINARY_DIR}/_sdl2image" EXCLUDE_FROM_ALL)

_find_tp(_IMGUI "Thirdparty/imgui")
if(NOT _IMGUI)
    message(FATAL_ERROR "找不到 imgui")
endif()
add_library(imgui STATIC
    "${_IMGUI}/imgui.cpp" "${_IMGUI}/imgui_draw.cpp"
    "${_IMGUI}/imgui_tables.cpp" "${_IMGUI}/imgui_widgets.cpp"
    "${_IMGUI}/backends/imgui_impl_sdl2.cpp"
    "${_IMGUI}/backends/imgui_impl_sdlrenderer2.cpp")
target_include_directories(imgui PUBLIC "${_IMGUI}" "${_IMGUI}/backends")
target_link_libraries(imgui PUBLIC SDL2-static)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(MSVC)
    set_source_files_properties(src/simd/KernelsAVX2.cpp PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
else()
    set_source_files_properties(src/simd/KernelsAVX2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2")
endif()

add_executable(simdrpg ${SRC})
target_include_directories(simdrpg PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
# SDL2_image 的头在源码树的 include/ 里，静态库目标不一定会导出它
if(SDL2_IMAGE_ROOT)
    target_include_directories(simdrpg PRIVATE "${SDL2_IMAGE_ROOT}/include")
endif()
if(TARGET SDL2-static)
    set(_sdl_tgt SDL2-static)
else()
    set(_sdl_tgt SDL2)
endif()
if(TARGET SDL2_image-static)
    set(_img_tgt SDL2_image-static)
else()
    set(_img_tgt SDL2_image)
endif()
target_link_libraries(simdrpg PRIVATE ${_sdl_tgt} ${_img_tgt} imgui)

set_target_properties(simdrpg PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")
