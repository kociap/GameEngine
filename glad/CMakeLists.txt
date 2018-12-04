cmake_minimum_required(VERSION 3.12)
project(glad C)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
   "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c"
   "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h"
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "glad"
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")