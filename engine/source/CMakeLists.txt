# TODO: scripts is a temporary directory to store a bunch of game scripts that e.g. move the camera around. Should probably remove soon.

add_library(anton_engine SHARED)

set_target_properties(anton_engine
    PROPERTIES
    FOLDER ${ENGINE_FOLDER_NAME}
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_compile_definitions(anton_engine
    PRIVATE 
    ENGINE_API=${ENGINE_DLL_EXPORT} 
    GAME_API=${ENGINE_DLL_IMPORT} 
    ANTON_WITH_EDITOR=${ENGINE_WITH_EDITOR}
    # Use unicode instead of multibyte charset (VS)
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
)

if(ENGINE_COMPILER_CLANG)
    target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_CLANG)
endif()

if(ENGINE_COMPILER_GCC)
    target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_GCC)
endif()

if(ENGINE_COMPILER_MSVC)
    target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_MSVC)
endif()

if(ENGINE_COMPILER_UNKNOWN)
    target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_UNKNOWN)
endif()

target_compile_options(anton_engine PRIVATE ${ENGINE_CXX_FLAGS})

target_include_directories(anton_engine 
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/public"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES FALSE "*.hpp")
file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES FALSE "*.cpp")
file(GLOB_RECURSE TMPL_IMPL_FILES LIST_DIRECTORIES FALSE "*.tpp")

if(WIN32)
    list(REMOVE_ITEM SOURCE_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/unix/debug_break.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/assert/assert_posix.cpp"
    )
else()
    list(REMOVE_ITEM SOURCE_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/debug_break.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/debugging.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/common.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/assert/assert_win.cpp"
    )

    list(REMOVE_ITEM HEADER_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/debugging.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/common.hpp"
    )
endif()

target_sources(anton_engine
    PRIVATE ${SOURCE_FILES} ${HEADER_FILES} ${TMPL_IMPL_FILES}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${HEADER_FILES} ${TMPL_IMPL_FILES})

if(UNIX)
    set(ENGINE_LINK_LIBS ${ENGINE_LINK_LIBS} dl)
endif()

target_link_libraries(anton_engine
    ${OPENGL_LIBRARIES}
    ${ENGINE_LINK_LIBS}
    glad
    zlib
    freetype
    mimas
)

if(WIN32)
    target_link_libraries(anton_engine
        DbgHelp
    )
endif()

set_target_properties(anton_engine
    PROPERTIES
    RUNTIME_OUTPUT_NAME_DEBUG "anton_engined"
    LIBRARY_OUTPUT_NAME_DEBUG "anton_engined"
    ARCHIVE_OUTPUT_NAME_DEBUG "anton_engined"
)


add_executable(anton_engine_loader "${CMAKE_CURRENT_SOURCE_DIR}/public/main.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${CMAKE_CURRENT_SOURCE_DIR}/public/main.cpp")

target_compile_definitions(anton_engine_loader 
    PRIVATE 
    ENGINE_API=${ENGINE_DLL_IMPORT} 
    GAME_API=${ENGINE_DLL_IMPORT} 
    ANTON_WITH_EDITOR=${ENGINE_WITH_EDITOR}
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
)

if(ENGINE_COMPILER_CLANG)
    target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_CLANG)
endif()

if(ENGINE_COMPILER_GCC)
    target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_GCC)
endif()

if(ENGINE_COMPILER_MSVC)
    target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_MSVC)
endif()

if(ENGINE_COMPILER_UNKNOWN)
    target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_UNKNOWN)
endif()

target_compile_options(anton_engine_loader PRIVATE ${ENGINE_CXX_FLAGS})

target_include_directories(anton_engine_loader 
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/public"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

set_target_properties(anton_engine_loader
    PROPERTIES
    FOLDER ${ENGINE_FOLDER_NAME}
    RUNTIME_OUTPUT_NAME_DEBUG "anton_engine_loaderd"
)

target_link_libraries(anton_engine_loader anton_engine)

# target_link_options(anton_engine_loader
#     PRIVATE ${ANTON_LINK_FLAGS} "clang_rt.asan-x86_64.lib"
# )

# target_link_options(anton_engine
#     PRIVATE ${ANTON_LINK_FLAGS} "clang_rt.asan_dll_thunk-x86_64.lib"
# )
