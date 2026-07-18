project(stb_image)

add_library(stb_image INTERFACE)

set(_stb_image_Sources
    stb_image.h
)

target_include_directories(stb_image INTERFACE ./)
target_sources(stb_image INTERFACE ${_stb_image_Sources})

source_group("ThirdParty\\stb_image" FILES ${_stb_image_Sources})