cmake_minimum_required(VERSION 3.25)
project(toyProjects)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

find_package(glm CONFIG REQUIRED)
add_subdirectory(thirdparty/SDL2/SDL-release-2.32.6 EXCLUDE_FROM_ALL)

set(COMPILER src/compiler/token.h)
set(PHONG_GAME src/PongGame/pong_game.cpp src/PongGame/pong_game.h)

add_executable(${PROJECT_NAME}
        thirdparty/opengl/src/glad.c main.cpp
        ${COMPILER}
        ${PHONG_GAME})

# SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

# OpenGL + GLM
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdparty/opengl/include)
link_directories(${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib/glfw3.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/opengl/lib/assimp-vc143-mt.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
