cmake_minimum_required(VERSION 3.25)
project(confps)
add_library(confps STATIC)
file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
target_sources(confps PRIVATE ${SOURCE})
target_include_directories(confps PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(confps PRIVATE SDL2::SDL2main SDL2::SDL2-static SDL2_ttf)