cmake_minimum_required(VERSION 3.25)
project(toyProjects)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# ==================> Projects file list <=====================
add_subdirectory(${CMAKE_SOURCE_DIR}/src/BouncyBall)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/PongGame)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Compiler)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/FluidSimulation)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Roguelike)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Madsycode)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/SDL2RayTracer)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/UI)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Tetris)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/CKong)
# ==================> Projects file list <=====================

# ==================> executable file <=====================
set(EXECUTABLE main.cpp command.h)
add_executable(${PROJECT_NAME} ${EXECUTABLE})
# ==================> executable file <=====================

# SDL2, SDL2_image must build upon SDL2::SDL2-static, no SDL2::SDL2
# =================== link SDL2, SDL2_image and raylib ===================
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/SDL2/SDL-release-2.32.6 EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/SDL2/SDL_image-release-2.8.0 EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/SDL2/SDL_ttf-release-2.20.0 EXCLUDE_FROM_ALL)
# =================== link SDL2, SDL2_image and raylib ===================

# =================== OpenGL + GLM + assimp ===================
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/opengl EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/glm EXCLUDE_FROM_ALL)
# =================== OpenGL + GLM + assimp ===================

# =================== imgui-node-editor-docking include: imgui, stb_image, imgui_node_editor ===================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/misc/cmake-modules)
find_package(imgui REQUIRED)
find_package(stb_image REQUIRED)
find_package(imgui_node_editor REQUIRED)
find_package(ScopeGuard REQUIRED)
# =================== imgui-node-editor-docking include: imgui, stb_image, imgui_node_editor ===================

# =================== TinyXML ===================
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/tinyxml)
# =================== TinyXML ===================

# =================== Link Projects ===================
target_link_libraries(${PROJECT_NAME} PUBLIC bouncyball)

target_link_libraries(${PROJECT_NAME} PUBLIC compiler)

target_link_libraries(${PROJECT_NAME} PUBLIC fluidsimulation)

target_link_libraries(${PROJECT_NAME} PUBLIC madsycode)

target_link_libraries(${PROJECT_NAME} PUBLIC pong)

target_link_libraries(${PROJECT_NAME} PUBLIC roguelike)

target_link_libraries(${PROJECT_NAME} PUBLIC sdl2raytracer)

target_link_libraries(imgui PUBLIC opengl SDL2::SDL2-static)
target_compile_definitions(imguiedit PUBLIC UIPlugin) # dll是否需要import
target_link_libraries(${PROJECT_NAME} PUBLIC imguiedit)

target_link_libraries(${PROJECT_NAME} PUBLIC imgui_001)

target_link_libraries(${PROJECT_NAME} PUBLIC tetris)
# =================== Link Projects ===================