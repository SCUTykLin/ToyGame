if (TARGET imgui_node_editor)
    return()
endif()

#set(_imgui_node_editor_SourceDir ${IMGUI_NODE_EDITOR_ROOT_DIR})
#set(_imgui_node_editor_BinaryDir ${CMAKE_BINARY_DIR}/NodeEditor)

#add_subdirectory(${_imgui_node_editor_SourceDir} ${_imgui_node_editor_BinaryDir})

find_package(imgui REQUIRED)

set(_imgui_node_editor_Sources
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/crude_json.cpp
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/crude_json.h
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_bezier_math.h
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_bezier_math.inl
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_canvas.cpp
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_canvas.cpp
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_canvas.h
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_canvas.h
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_extra_math.h
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_extra_math.inl
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_node_editor_api.cpp
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_node_editor_internal.h
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_node_editor_internal.inl
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_node_editor.cpp
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/imgui_node_editor.h
        ${CMAKE_SOURCE_DIR}/Thirdparty/imgui-node-editor/misc/imgui_node_editor.natvis
)

add_library(imgui_node_editor STATIC
    ${_imgui_node_editor_Sources}
)

target_include_directories(imgui_node_editor PUBLIC
    ${_imgui_node_editor_Sources}
)

target_link_libraries(imgui_node_editor PUBLIC imgui)

source_group(TREE ${IMGUI_NODE_EDITOR_ROOT_DIR} FILES ${_imgui_node_editor_Sources})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui_node_editor
    REQUIRED_VARS
        IMGUI_NODE_EDITOR_ROOT_DIR
)