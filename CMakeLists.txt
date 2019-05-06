cmake_minimum_required(VERSION 3.12)
project(GameEngine)

enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)

set(GAMEENGINE_FOLDER_NAME "Engine")
set(ENGINE_DEPENDENCIES_FOLDER "Dependencies")

find_package(OpenGL REQUIRED)
add_subdirectory(engine)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
elseif("${CMAKE_CXX_COMPILER_ID}" matches "GCC" or "${CMAKE_CXX_COMPILER_ID}" matches "Clang")

endif()