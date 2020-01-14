set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_window.cpp"
)
    
set(HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/editor_window.hpp"
)

add_subdirectory(editor/public)
add_subdirectory(editor/private)
add_subdirectory(imgui/public)
add_subdirectory(imgui/private)
# add_subdirectory(level_editor/public)
# add_subdirectory(level_editor/private)
add_subdirectory(content_browser/public)
add_subdirectory(content_browser/private)
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
