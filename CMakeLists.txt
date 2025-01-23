cmake_minimum_required(VERSION 3.18)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

project(anton_engine)

enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)

# TODO: Remove
option(DESERIALIZE "Load scene from file instead of generate through code. Legacy option" OFF)
option(ENGINE_BUILD_EDITOR "Build the engine with the editor" ON)
option(ENGINE_BUILD_TOOLS "Build additional tools" OFF)
option(ENGINE_BUILD_WITH_ASAN "Build the engine with Address Sanitizer (Clang only)" OFF)

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

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set(ANTON_COMPILE_FLAGS
    -fno-math-errno
    -fno-char8_t
    -Wall -Wextra -pedantic
    -ferror-limit=1
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

    -Werror=return-type

    -Wno-implicit-fallthrough # there's implicit falthrough in murmurhash2.hpp

    -Wno-error=unused-const-variable
    -Wno-error=unused-parameter
    -Wno-error=unused-variable
  )

  set(ENGINE_LINK_LIBS
    stdc++fs
  )

  set(ENGINE_DEPENDENCIES_COMPILE_FLAGS -Wno-everything)
  set(ENGINE_DLL_EXPORT "")
  set(ENGINE_DLL_IMPORT "")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND NOT MSVC)
  # Compiling with Clang not simulating MSVC
  set(ANTON_COMPILE_FLAGS
    -fno-math-errno
    -fno-char8_t
    -Wall -Wextra -pedantic
    -ferror-limit=1
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
    -Wnewline-eof

    -Werror=return-type

    -Wno-implicit-fallthrough # there's implicit falthrough in murmurhash2.hpp

    -Wno-error=unused-const-variable
    -Wno-error=unused-parameter
    -Wno-error=unused-variable
  )

  set(ENGINE_DEPENDENCIES_COMPILE_FLAGS -Wno-everything)

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

    set(ENGINE_DEPENDENCIES_COMPILE_FLAGS ${ENGINE_DEPENDENCIES_COMPILE_FLAGS} "-fsanitize=address")
    set(ANTON_DEPENDENCIES_LINK_FLAGS "-fsanitize=address" "-shared-libasan")
  endif()
elseif(MSVC)
  set(ANTON_COMPILE_FLAGS
    /W3
    # Extended diagnostics include the column where the issue was found and
    # places a caret (^) under the location.
    /diagnostics:caret
    /permissive-
  )

  set(ENGINE_LINK_LIBS)

  set(ENGINE_DEPENDENCIES_COMPILE_FLAGS /W0)
  set(ENGINE_DLL_EXPORT "__declspec(dllexport)")
  set(ENGINE_DLL_IMPORT "__declspec(dllimport)")
endif()

# DEPENDENCIES

# Add anton_core
FetchContent_Declare(
  anton_core
  GIT_REPOSITORY https://github.com/kociap/anton_core.git
  GIT_TAG 043402b895783b7981bbdc58b78c4fca07f96451
)
FetchContent_MakeAvailable(anton_core)
# Add zlib
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/kociap/zlib.git
  GIT_TAG 84e627736f5b208731deb3fbffcd957734b6e049
)
FetchContent_MakeAvailable(zlib)
target_compile_options(zlib PRIVATE ${ENGINE_DEPENDENCIES_COMPILE_FLAGS})
# Add glad
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/kociap/glad.git
  GIT_TAG b4d4f69539196fdb08a56b88fd15b7287c838b87
)
FetchContent_MakeAvailable(glad)
# Add mimas
FetchContent_Declare(
  mimas
  GIT_REPOSITORY https://github.com/kociap/mimas.git
  GIT_TAG cae14fec321a5816df70c1cc444681016b04f9f1
)
FetchContent_MakeAvailable(mimas)
# Add FreeType. Disable the HarfBuzz backend.
set(FT_WITH_HARFBUZZ OFF)
set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz TRUE)
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/kociap/FreeType.git
  GIT_TAG 57f8be4549ce5ef59ca4d4e662403bc2d11302af
)
FetchContent_MakeAvailable(freetype)
# Add HarfBuzz. We currently do not use it, hence we leave disabled. Do not
# build with FreeType.
# set(HB_HAVE_FREETYPE OFF)
# FetchContent_Declare(
#   harfbuzz
#   GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
#   GIT_TAG 7b27c8edd46c674e01dd226fa9e1aa7549f5c436 # Version 10.2.0
# )
# FetchContent_MakeAvailable(harfbuzz)

# SUBDIRECTORIES

add_subdirectory(engine)
if(${ENGINE_BUILD_EDITOR})
    add_subdirectory(editor)
endif()
if(${ENGINE_BUILD_TOOLS})
    add_subdirectory(tools)
endif()
