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
    ANTON_COMPILER_ID=${ENGINE_COMPILER_ID} 
    ANTON_COMPILER_CLANG=${ENGINE_COMPILER_CLANG} 
    ANTON_COMPILER_GCC=${ENGINE_COMPILER_GCC} 
    ANTON_COMPILER_MSVC=${ENGINE_COMPILER_MSVC} 
    ANTON_COMPILER_UNKNOWN=${ENGINE_COMPILER_UNKNOWN}
    ANTON_WITH_EDITOR=${ENGINE_WITH_EDITOR}
    # Use unicode instead of multibyte charset (VS)
    UNICODE
    _UNICODE
)

target_compile_options(anton_engine
    PRIVATE ${ENGINE_CXX_FLAGS}
)

target_include_directories(anton_engine 
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/public"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES FALSE "*.hpp")
file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES FALSE "*.cpp")
file(GLOB_RECURSE TMPL_IMPL_FILES LIST_DIRECTORIES FALSE "*.tpp")

if(WIN32)
    list(REMOVE_ITEM SOURCE_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/assert/assert_posix.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/time/time_posix.cpp"
        )
else()
    list(REMOVE_ITEM SOURCE_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/debugging.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/common.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/core/assert/assert_win.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/time/time_win.cpp"
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

target_link_libraries(anton_engine
    ${OPENGL_LIBRARIES}
    glad
    zlibstatic
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
    ANTON_COMPILER_ID=${ENGINE_COMPILER_ID} 
    ANTON_COMPILER_CLANG=${ENGINE_COMPILER_CLANG} 
    ANTON_COMPILER_GCC=${ENGINE_COMPILER_GCC} 
    ANTON_COMPILER_MSVC=${ENGINE_COMPILER_MSVC} 
    ANTON_COMPILER_UNKNOWN=${ENGINE_COMPILER_UNKNOWN}
    ANTON_WITH_EDITOR=${ENGINE_WITH_EDITOR}
)

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
#     PRIVATE /PROFILE
# )

# target_link_options(${ENGINE_NAME}
#     PRIVATE /PROFILE
# )
