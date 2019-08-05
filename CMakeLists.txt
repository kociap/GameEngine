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

if(MSVC)
    set(ENGINE_CXX_FLAGS "/W4")
    set(ENGINE_DLL_EXPORT "__declspec(dllexport)")
    set(ENGINE_DLL_IMPORT "__declspec(dllimport)")
elseif("${CMAKE_CXX_COMPILER_ID}" matches "GNU" or "${CMAKE_CXX_COMPILER_ID}" matches "Clang")
    set(ENGINE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-error=unused-const-variable -Wno-error=unused-parameter -Wno-error=unused-variable")
    set(ENGINE_DLL_EXPORT "")
    set(ENGINE_DLL_IMPORT "")
endif()

add_subdirectory(engine)
