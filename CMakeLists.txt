cmake_minimum_required(VERSION 3.12)
project(GameEngine)

enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)

set(ENGINE_NAME ${PROJECT_NAME})
set(ENGINE_BINARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(ENGINE_LOADER_NAME "${PROJECT_NAME}Loader")
set(ENGINE_FOLDER_NAME "Engine")
set(ENGINE_DEPENDENCIES_FOLDER "Dependencies")
set(ENGINE_TOOLS_FOLDER "Tools")

add_subdirectory(engine)

set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS ON)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
elseif("${CMAKE_CXX_COMPILER_ID}" matches "GCC" or "${CMAKE_CXX_COMPILER_ID}" matches "Clang")

endif()