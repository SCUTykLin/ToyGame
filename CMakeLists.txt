cmake_minimum_required(VERSION 3.25)
project(toyProjects)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(thirdparty/SDL2/SDL-release-2.32.6 EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/raylib)


set(COMPILER src/compiler/token.h)
set(PHONG_GAME src/PongGame/pong_game.cpp src/PongGame/pong_game.h)
set(BOUNCY_BALL
        src/BouncyBall/bouncy_ball.cpp src/BouncyBall/bouncy_ball.h
        src/BouncyBall/window_tool.cpp src/BouncyBall/window_tool.h)
set(FLUID_SIMULATION
        src/FluidSimulation/fluid.cpp
        src/FluidSimulation/fluid.h)

# raylib自带了一个glad，不需要添加 thirdparty/src/glad.c
set(EXECUTABLE main.cpp main.h)

add_executable(${PROJECT_NAME}
        ${EXECUTABLE}
        ${COMPILER}
        ${PHONG_GAME}
        ${BOUNCY_BALL})

# SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

# OpenGL + GLM + assimp
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdparty/opengl/include)
link_directories(${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib/glfw3.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib/assimp-vc143-mt.lib)

# raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)