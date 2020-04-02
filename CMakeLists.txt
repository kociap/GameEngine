cmake_minimum_required(VERSION 3.13)
project(anton_engine)

enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)

set(ENGINE_BINARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(ENGINE_FOLDER_NAME "Engine")
set(ENGINE_DEPENDENCIES_FOLDER "Dependencies")
set(ENGINE_TOOLS_FOLDER "Tools")

set(ENGINE_WITH_EDITOR 1)
set(ANTON_BUILD_WITH_ASAN TRUE)
set(ANTON_BUILD_TOOLS FALSE)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(ANTON_COMPILE_FLAGS
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
        # -Wno-shadow-field-in-constructor
        # -Wno-missing-prototypes
        -Wnon-virtual-dtor
        -Wno-implicit-fallthrough # there's implicit falthrough in murmurhash2.hpp
        -Werror=return-type
        -Wno-error=unused-const-variable 
        -Wno-error=unused-parameter
        -Wno-error=unused-variable
    )

    set(ENGINE_LINK_LIBS
        stdc++fs
    )

    set(ANTON_DEPENDENCIES_COMPILE_FLAGS -Wno-everything)
    set(ENGINE_DLL_EXPORT "")
    set(ENGINE_DLL_IMPORT "")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND NOT MSVC)
    # Compiling with Clang not simulating MSVC
    set(ANTON_COMPILE_FLAGS
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
        # -Wno-shadow-field-in-constructor
        # -Wno-missing-prototypes
        -Wnewline-eof
        -Wnon-virtual-dtor
        -Wno-error=unused-const-variable 
        -Wno-error=unused-parameter
        -Wno-error=unused-variable
        -Werror=return-type
        # -ftime-trace
        -fno-rtti
    )

    set(ANTON_DEPENDENCIES_COMPILE_FLAGS -Wno-everything)

    if(ANTON_BUILD_WITH_ASAN)
        set(ANTON_COMPILE_FLAGS ${ANTON_COMPILE_FLAGS}
            -fsanitize=address
            -fsanitize-address-use-after-scope
            -O0
            -g3
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls
        )
        set(ANTON_LINK_FLAGS "-fsanitize=address" "-shared-libasan")

        set(ANTON_DEPENDENCIES_COMPILE_FLAGS ${ANTON_DEPENDENCIES_COMPILE_FLAGS} "-fsanitize=address")
        set(ANTON_DEPENDENCIES_LINK_FLAGS "-fsanitize=address" "-shared-libasan")
    endif()

    
    if(NOT WIN32)
        set(ENGINE_DLL_EXPORT "")
        set(ENGINE_DLL_IMPORT "")
    else()
        set(ENGINE_DLL_EXPORT "__declspec(dllexport)")
        set(ENGINE_DLL_IMPORT "__declspec(dllimport)")
    endif()
elseif(MSVC)
    set(ANTON_COMPILE_FLAGS 
        /W3
        /diagnostics:caret # Extended diagnostics include the column where the issue was found and places a caret (^) under the location
        /permissive-
    )

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(ANTON_COMPILE_FLAGS
            ${ANTON_COMPILE_FLAGS}
            -Wnewline-eof
            -Wnon-virtual-dtor
        )
    endif()

    set(ENGINE_LINK_LIBS)

    set(ANTON_DEPENDENCIES_COMPILE_FLAGS /W0)
    set(ENGINE_DLL_EXPORT "__declspec(dllexport)")
    set(ENGINE_DLL_IMPORT "__declspec(dllimport)")
endif()

# Compilers
set(ENGINE_COMPILER_UNKNOWN FALSE)
set(ENGINE_COMPILER_CLANG FALSE)
set(ENGINE_COMPILER_GCC FALSE)
set(ENGINE_COMPILER_MSVC FALSE)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(ENGINE_COMPILER_CLANG TRUE)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(ENGINE_COMPILER_GCC TRUE)
elseif(MSVC)
    set(ENGINE_COMPILER_MSVC TRUE)
else()
    set(ENGINE_COMPILER_UNKNOWN TRUE)
endif()

add_subdirectory(engine)
