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

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND NOT MSVC))
    # Compiling with GCC or Clang not simulating MSVC
    set(ENGINE_CXX_FLAGS 
        -ferror-limit=512 # Lemme see dem errors
        -Wall -Wextra -pedantic
        # Useful warnings that are included in -Weverything
        # -Wsign-conversion
        # -Wshorten-64-to-32
        # -Wimplicit-int-conversion
        # -Wfloat-conversion

        # -Wno-c++98-compat -Wno-c++98-compat-pedantic # Both are useless
        # -Wno-float-equal # Not important yet
        # -Wno-global-constructors # Suppress static variables initialization warning spam. Not important, but might be worth turning on to identify startup performance issues
        # -Wno-exit-time-destructors # Suppress static variables destruction warning spam.
        # -Wno-shadow-field-in-constructor # TODO temporarily to suppress spam
        # -Wno-missing-prototypes
        -Wnewline-eof
        -Wno-error=unused-const-variable 
        -Wno-error=unused-parameter
        -Wno-error=unused-variable
    )

    set(ENGINE_CXX_DEPENDENCIES_FLAGS -Wno-everything)
    set(ENGINE_DLL_EXPORT "")
    set(ENGINE_DLL_IMPORT "")
elseif(MSVC)
    set(ENGINE_CXX_FLAGS 
        /W4
        /diagnostics:caret # Extended diagnostics include the column where the issue was found and places a caret (^) under the location
        /permissive-
    )

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(ENGINE_CXX_FLAGS
            ${ENGINE_CXX_FLAGS}
            -Wnewline-eof
        )
    endif()

    set(ENGINE_CXX_DEPENDENCIES_FLAGS /W0)
    set(ENGINE_DLL_EXPORT "__declspec(dllexport)")
    set(ENGINE_DLL_IMPORT "__declspec(dllimport)")
endif()

# Compiler IDs
set(ENGINE_COMPILER_UNKNOWN 0)
set(ENGINE_COMPILER_CLANG 1)
set(ENGINE_COMPILER_GCC 2)
set(ENGINE_COMPILER_MSVC 3)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(ENGINE_COMPILER_ID ${ENGINE_COMPILER_CLANG})
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(ENGINE_COMPILER_ID ${ENGINE_COMPILER_GCC})
elseif(MSVC)
    set(ENGINE_COMPILER_ID ${ENGINE_COMPILER_MSVC})
else()
    set(ENGINE_COMPILER_ID ${ENGINE_COMPILER_UNKNOWN})
endif()

add_subdirectory(engine)
