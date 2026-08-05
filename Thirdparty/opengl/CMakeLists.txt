cmake_minimum_required(VERSION 3.25)
project(opengl)
add_library(opengl)

find_package(OpenGL REQUIRED)

target_sources(opengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(opengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib)
target_link_libraries(opengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc143-mt.lib OpenGL::GL)
target_include_directories(opengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)