cmake_minimum_required(VERSION 3.12)
project(stb CXX)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/stb/stb_image.h"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "stb"
    LINKER_LANGUAGE CXX
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")