cmake_minimum_required(VERSION 3.25)
project(texteditor)
add_library(texteditor STATIC)

file(GLOB_RECURSE SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

target_sources(texteditor PRIVATE ${SOURCE_FILE})

target_include_directories(texteditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(texteditor PRIVATE SDL2::SDL2main SDL2::SDL2-static SDL2_ttf stb_image)
