set(HEADER_FILES
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/camera_movement.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_hotkeys.hpp"
    PARENT_SCOPE
)

set(SOURCE_FILES
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/camera_movement.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_hotkeys.cpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")