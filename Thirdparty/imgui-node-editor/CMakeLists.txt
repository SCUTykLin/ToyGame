cmake_minimum_required(VERSION 3.25)
project(ImGuiNodeEditor)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Point CMake where to look for module files.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Support/CMakeModules)

set(CMAKE_CXX_STANDARD            20)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

if (DEFINED MSVC_VERSION)
    if (MSVC_VERSION GREATER_EQUAL 1500)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
    endif()
    if (MSVC_VERSION GREATER_EQUAL 1910)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
    endif()
endif()

add_subdirectory(Examples)
