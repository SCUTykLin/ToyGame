cmake_minimum_required(VERSION 3.25)
project(toyProjects)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(thirdparty/SDL2/SDL-release-2.32.6 EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/raylib)

# imgui
set(IMGUI_DIR
        thirdparty/imgui/src/imgui.cpp
        thirdparty/imgui/src/imgui_demo.cpp
        thirdparty/imgui/src/imgui_draw.cpp
        thirdparty/imgui/src/imgui_tables.cpp
        thirdparty/imgui/src/imgui_widgets.cpp
        thirdparty/imgui/src/imgui_impl_glfw.cpp
        thirdparty/imgui/src/imgui_impl_opengl3.cpp)
# stb_image
set(STB_IMAGE
        thirdparty/stb_image/src/stb_image.cpp)

# Projects:
set(COMPILER
        src/Compiler/app.cpp src/Compiler/app.h
        src/Compiler/compiler/compiler.cpp src/Compiler/compiler/compiler.h
        src/Compiler/Lexer/lexer.cpp src/Compiler/Lexer/lexer.h)

set(PHONG_GAME
        src/PongGame/pong_game.cpp src/PongGame/pong_game.h)

set(BOUNCY_BALL
        src/BouncyBall/bouncy_ball.cpp src/BouncyBall/bouncy_ball.h
        src/BouncyBall/window_tool.cpp src/BouncyBall/window_tool.h)

set(FLUID_SIMULATION
        src/FluidSimulation/window_tool.cpp src/FluidSimulation/window_tool.h
        src/FluidSimulation/App/FluidApplication.cpp src/FluidSimulation/App/FluidApplication.h
        src/FluidSimulation/Grid/Grid.cpp src/FluidSimulation/Grid/Grid.h)

# raylib自带了一个glad，不需要添加 thirdparty/src/glad.c
set(EXECUTABLE main.cpp main.h ${IMGUI_DIR} ${STB_IMAGE})
add_executable(${PROJECT_NAME}
        ${EXECUTABLE}
        ${COMPILER}
        ${PHONG_GAME}
        ${BOUNCY_BALL}
        ${FLUID_SIMULATION})

# SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

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