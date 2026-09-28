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
add_subdirectory(${CMAKE_SOURCE_DIR}/src/FluidSimulation)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Roguelike)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Madsycode)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/SDL2Math)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/UI)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Tetris)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/ConFPS)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Interpreter_v1)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/UpFPS)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/RacingGame)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/olcEngin)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Frogger)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/TextEditor)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Asteroids)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Worms)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/TileBasedPlatform)
# ==================> Projects file list <=====================

# ==================> executable file <=====================
set(EXECUTABLE main.cpp command.h command.cpp)
add_executable(${PROJECT_NAME} ${EXECUTABLE})
# ==================> executable file <=====================

# SDL2, SDL2_image must build upon SDL2::SDL2-static, no SDL2::SDL2
# =================== link SDL2, SDL2_image and raylib ===================
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2TTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/SDL2/SDL-release-2.32.6 EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/SDL2/SDL_image-release-2.8.0 EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/SDL2/SDL_ttf-release-2.20.0 EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/ImFileDialog EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/Thirdparty/fltk-1.4.5 EXCLUDE_FROM_ALL)
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
target_link_libraries(${PROJECT_NAME} PRIVATE bouncyball)

target_link_libraries(${PROJECT_NAME} PRIVATE fluidsimulation)

target_link_libraries(${PROJECT_NAME} PRIVATE madsycode)

target_link_libraries(${PROJECT_NAME} PRIVATE pong)

target_link_libraries(${PROJECT_NAME} PRIVATE roguelike)

target_link_libraries(${PROJECT_NAME} PRIVATE sdl2math)

target_link_libraries(imgui PRIVATE opengl SDL2::SDL2-static) # imgui的后端有SDL2
target_compile_definitions(imguiedit PRIVATE UIPlugin) # dll是否需要import

target_link_libraries(${PROJECT_NAME} PRIVATE imguiedit)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui_001)
target_link_libraries(${PROJECT_NAME} PRIVATE tetris)
target_link_libraries(${PROJECT_NAME} PRIVATE confps)
target_link_libraries(${PROJECT_NAME} PRIVATE Interpreter_v1)
target_link_libraries(${PROJECT_NAME} PRIVATE upfps)
target_link_libraries(${PROJECT_NAME} PRIVATE racing)
target_link_libraries(${PROJECT_NAME} PRIVATE Frogger)
target_link_libraries(${PROJECT_NAME} PRIVATE texteditor)
target_link_libraries(${PROJECT_NAME} PRIVATE asteroids)
target_link_libraries(${PROJECT_NAME} PRIVATE worms)
target_link_libraries(${PROJECT_NAME} PRIVATE tilebased)
# =================== Link Projects ===================
target_link_libraries(${PROJECT_NAME} PRIVATE fltk::fltk)
