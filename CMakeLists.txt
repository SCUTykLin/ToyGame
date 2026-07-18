cmake_minimum_required(VERSION 3.25)
project(toyProjects)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# ==================> Projects file list <=====================
file(GLOB_RECURSE COMPILER src/Compiler/*.cpp src/Compiler/*.h)
file(GLOB_RECURSE PHONG_GAME src/PongGame/*.cpp src/PongGame/*.h)
file(GLOB_RECURSE BOUNCY_BALL src/BouncyBall/*.cpp src/BouncyBall/*.h)
file(GLOB_RECURSE FLUID_SIMULATION src/FluidSimulation/*.cpp src/FluidSimulation/*.h)
file(GLOB_RECURSE ROGUELIKE src/Roguelike/*.cpp src/Roguelike/*.h)
file(GLOB_RECURSE MADSYCODE src/MADSYCODE/*.cpp src/MADSYCODE/*.h)
file(GLOB_RECURSE UI src/UI/*.cpp UI src/UI/*.h)
# ==================> Projects file list <=====================

# ==================> executable file <=====================
set(EXECUTABLE Thirdparty/opengl/src/glad.c
        main.cpp command.h opengL_test.h)
add_executable(${PROJECT_NAME}
        ${EXECUTABLE}
        ${COMPILER}
        ${PHONG_GAME}
        ${BOUNCY_BALL}
        ${FLUID_SIMULATION}
        ${ROGUELIKE}
        ${MADSYCODE}
        ${UI})
# ==================> executable file <=====================

# SDL2, SDL2_image must build upon SDL2::SDL2-static, no SDL2::SDL2
# =================== link SDL2, SDL2_image and raylib ===================
add_subdirectory(Thirdparty/SDL2/SDL-release-2.32.6 EXCLUDE_FROM_ALL)
add_subdirectory(Thirdparty/SDL2/SDL_image-release-2.8.0 EXCLUDE_FROM_ALL)
add_subdirectory(Thirdparty/SDL2/SDL_ttf-release-2.20.0 EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2main SDL2::SDL2-static SDL2_image SDL2_ttf)
# =================== link SDL2, SDL2_image and raylib ===================

# =================== OpenGL + GLM + assimp ===================
include_directories(${PROJECT_SOURCE_DIR}/Thirdparty/opengl/include)
link_directories(${PROJECT_SOURCE_DIR}/Thirdparty/opengl/lib)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/Thirdparty/opengl/lib/glfw3.lib)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/Thirdparty/opengl/lib/assimp-vc143-mt.lib)
# =================== OpenGL + GLM + assimp ===================

# =================== imgui-node-editor-docking include: imgui, stb_image, imgui_node_editor ===================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/misc/cmake-modules)
find_package(imgui REQUIRED)
find_package(stb_image REQUIRED)
find_package(imgui_node_editor REQUIRED)
find_package(ScopeGuard REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui stb_image imgui_node_editor ScopeGuard)
# =================== imgui-node-editor-docking include: imgui, stb_image, imgui_node_editor ===================


# dll是否需要import
target_compile_definitions(${PROJECT_NAME} PUBLIC UIPlugin)