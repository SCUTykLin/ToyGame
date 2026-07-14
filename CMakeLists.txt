cmake_minimum_required(VERSION 3.25)
project(toyProjects)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(thirdparty/SDL2/SDL-release-2.32.6 EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/SDL2/SDL_image-release-2.8.0 EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/raylib)

# imgui
file(GLOB IMGUI_DIR thirdparty/imgui/src/*.cpp)

# stb_image
file(GLOB_RECURSE STB_IMAGE thirdparty/stb_image/*.cpp thirdparty/stb_image/*.h)

# Projects file list:
file(GLOB_RECURSE COMPILER src/Compiler/*.cpp src/Compiler/*.h)
file(GLOB_RECURSE PHONG_GAME src/PongGame/*.cpp src/PongGame/*.h)
file(GLOB_RECURSE BOUNCY_BALL src/BouncyBall/*.cpp src/BouncyBall/*.h)
file(GLOB_RECURSE FLUID_SIMULATION src/FluidSimulation/*.cpp src/FluidSimulation/*.h)
file(GLOB_RECURSE ROGUELIKE src/Roguelike/*.cpp src/Roguelike/*.h)

# raylib自带了一个glad，不需要添加 thirdparty/src/glad.c
set(EXECUTABLE main.cpp main.h
        ${IMGUI_DIR}
        ${STB_IMAGE})
add_executable(${PROJECT_NAME}
        ${EXECUTABLE}
        ${COMPILER}
        ${PHONG_GAME}
        ${BOUNCY_BALL}
        ${FLUID_SIMULATION}
        ${ROGUELIKE})

# SDL2. SDL2_image must build upon SDL2::SDL2-static, no SDL2::SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2-static SDL2_image)

# imgui + stb_image
target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/thirdparty/imgui/include)
target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/thirdparty/stb_image/include)
# OpenGL + GLM + assimp
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdparty/opengl/include)
link_directories(${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib/glfw3.lib)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib/assimp-vc143-mt.lib)

# raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)