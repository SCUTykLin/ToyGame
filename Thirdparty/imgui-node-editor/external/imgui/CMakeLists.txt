project(imgui)

file(GLOB HEAD include/*.h)
file(GLOB SOURCE src/*.cpp)

set(_imgui_Sources ${HEAD} ${SOURCE})

source_group("" FILES ${_imgui_Sources})

add_library(${PROJECT_NAME} STATIC ${_imgui_Sources})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_compile_definitions(${PROJECT_NAME} PUBLIC "ImDrawIdgx=unsigned int")

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "ThirdParty")