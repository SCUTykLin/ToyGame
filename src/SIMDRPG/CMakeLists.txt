
# game处理游戏逻辑 -> ecs场景管理，更新 -> gfx发出绘制请求 -> simd混合像素 -> platform平台渲染

cmake_minimum_required(VERSION 3.20)
project(simdrpg CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

if(MSVC)
    add_compile_options(/utf-8 /W3)
    add_compile_definitions(SDL_MAIN_HANDLED)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

function(simdrpg_find_thirdparty out_var rel)
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

simdrpg_find_thirdparty(_SDL2_SRC "Thirdparty/SDL2/SDL-release-2.32.6")
simdrpg_find_thirdparty(_SDL2IMG_SRC "Thirdparty/SDL2/SDL_image-release-2.8.0")

find_package(SDL2 CONFIG QUIET)

if(SDL2_FOUND)
    set(SIMDRPG_SDL_TARGET SDL2::SDL2-static)
    if(NOT TARGET SDL2::SDL2-static)
        set(SIMDRPG_SDL_TARGET SDL2::SDL2)
    endif()
else()
    set(SDL2_ROOT "${_SDL2_SRC}" CACHE PATH "SDL2 源码目录")
    if(NOT EXISTS "${SDL2_ROOT}/CMakeLists.txt")
        message(FATAL_ERROR
            "找不到 SDL2。两条路任选一条：\n"
            "  1) 安装 SDL2 后重新 configure，并设置 -DSDL2_DIR=<含 SDL2Config.cmake 的目录>\n"
            "  2) 把 SDL2 源码放到任意上层目录的 Thirdparty/SDL2/SDL-release-2.32.6，"
            "或设置 -DSDL2_ROOT=<源码目录>")
    endif()
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON  CACHE BOOL "" FORCE)
    set(SDL_TEST   OFF CACHE BOOL "" FORCE)

    add_subdirectory("${SDL2_ROOT}" "${CMAKE_BINARY_DIR}/_sdl2" EXCLUDE_FROM_ALL)
    if(TARGET SDL2-static)
        set(SIMDRPG_SDL_TARGET SDL2-static)
    else()
        set(SIMDRPG_SDL_TARGET SDL2)
    endif()
endif()

# SDL2_image 同理
find_package(SDL2_image CONFIG QUIET)
if(SDL2_image_FOUND)
    # find_package 的目标名各版本不同，按存在的挑
    if(TARGET SDL2_image::SDL2_image-static)
        set(SIMDRPG_SDLIMAGE_TARGET SDL2_image::SDL2_image-static)
    else()
        set(SIMDRPG_SDLIMAGE_TARGET SDL2_image::SDL2_image)
    endif()
else()
    set(SDL2_IMAGE_ROOT "${_SDL2IMG_SRC}" CACHE PATH "SDL2_image 源码目录")
    if(NOT EXISTS "${SDL2_IMAGE_ROOT}/CMakeLists.txt")
        message(FATAL_ERROR
            "找不到 SDL2_image。设置 -DSDL2_IMAGE_ROOT=<源码目录>，"
            "或放到上层目录的 Thirdparty/SDL2/SDL_image-release-2.8.0")
    endif()
    set(SDL2IMAGE_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_STATIC ON  CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory("${SDL2_IMAGE_ROOT}" "${CMAKE_BINARY_DIR}/_sdl2image" EXCLUDE_FROM_ALL)
    # 源码构建时目标名是 SDL2_image（2.8 起不再带 -static 后缀）
    if(TARGET SDL2_image-static)
        set(SIMDRPG_SDLIMAGE_TARGET SDL2_image-static)
    elseif(TARGET SDL2_image)
        set(SIMDRPG_SDLIMAGE_TARGET SDL2_image)
    else()
        message(FATAL_ERROR "SDL2_image 目标名未知，请查看其 CMakeLists 的 add_library")
    endif()
endif()

simdrpg_find_thirdparty(_IMGUI_SRC "Thirdparty/imgui")
if(NOT _IMGUI_SRC)
    message(FATAL_ERROR
        "找不到 ImGui。把 imgui 源码放到上层目录的 Thirdparty/imgui，"
        "或设置 -D_IMGUI_SRC=<imgui 目录>")
endif()

add_library(imgui STATIC
    "${_IMGUI_SRC}/imgui.cpp"
    "${_IMGUI_SRC}/imgui_draw.cpp"
    "${_IMGUI_SRC}/imgui_tables.cpp"
    "${_IMGUI_SRC}/imgui_widgets.cpp"
    "${_IMGUI_SRC}/backends/imgui_impl_sdl2.cpp"
    "${_IMGUI_SRC}/backends/imgui_impl_sdlrenderer2.cpp")

target_include_directories(imgui PUBLIC "${_IMGUI_SRC}" "${_IMGUI_SRC}/backends")
target_link_libraries(imgui PUBLIC ${SIMDRPG_SDL_TARGET})

file(GLOB_RECURSE SIMDRPG_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(NOT SIMDRPG_SOURCES)
    message(FATAL_ERROR
        "src/ 下没有任何 .cpp。\n"
        "先从 03a 阶段 0 开始，把 Window.cpp 和 main.cpp 建出来。")
endif()

add_executable(simdrpg ${SIMDRPG_SOURCES})

target_include_directories(simdrpg PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
if(SDL2_IMAGE_ROOT)
    target_include_directories(simdrpg PRIVATE "${SDL2_IMAGE_ROOT}/include")
endif()
target_link_libraries(simdrpg PRIVATE ${SIMDRPG_SDL_TARGET} ${SIMDRPG_SDLIMAGE_TARGET} imgui)

# 只有这个文件开 AVX2 指令集。
# 全项目开 /arch:AVX2 的话，程序在没 AVX2 的机器上会直接崩（非法指令），
# 那 CPU 探测就白写了。必须"单文件开"。
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/simd/KernelsAVX2.cpp")
    if(MSVC)
        set_source_files_properties(src/simd/KernelsAVX2.cpp
            PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
    else()
        set_source_files_properties(src/simd/KernelsAVX2.cpp
            PROPERTIES COMPILE_OPTIONS "-mavx2")
    endif()
endif()

# 产物统一放到 build/bin/，多配置生成器（VS）也不会跑到 bin/Release 去
set_target_properties(simdrpg PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
    VS_DEBUGGER_WORKING_DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}")
