set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_events.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_input_filter.cpp"
)
    
set(HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_events.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_subsystem_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_window.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_input_filter.hpp"
)

add_subdirectory(level_editor/public)
add_subdirectory(level_editor/private)
add_subdirectory(misc/public)
add_subdirectory(misc/private)
add_subdirectory(widgets/public)
add_subdirectory(widgets/private)
add_subdirectory(content_browser/public)
add_subdirectory(content_browser/private)
add_subdirectory(outliner/public)
add_subdirectory(outliner/private)
add_subdirectory(log_viewer/public)
add_subdirectory(log_viewer/private)
add_subdirectory(rendering/public)
add_subdirectory(rendering/private)

set(SOURCE_FILES_ROOT
    ${SOURCE_FILES_ROOT}
    ${SOURCE_FILES}
    PARENT_SCOPE
)

set(HEADER_FILES_ROOT
    ${HEADER_FILES_ROOT}
    ${HEADER_FILES}
    PARENT_SCOPE
)

set(PRIVATE_INCLUDE_DIRECTORIES_ROOT
    ${PRIVATE_INCLUDE_DIRECTORIES_ROOT}
    ${PRIVATE_INCLUDE_DIRECTORIES}
    PARENT_SCOPE
)

set(PUBLIC_INCLUDE_DIRECTORIES_ROOT
    ${PUBLIC_INCLUDE_DIRECTORIES_ROOT}
    ${PUBLIC_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)
