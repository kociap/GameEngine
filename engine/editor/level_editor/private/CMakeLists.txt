set(SOURCE_FILES
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/gizmo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewport_camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewport.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/gizmo_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewport_camera.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")