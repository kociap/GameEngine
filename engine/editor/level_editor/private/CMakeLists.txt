set(SOURCE_FILES
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/gizmo/gizmo_types.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/gizmo/gizmo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/gizmo_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewport_camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewport.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES 
    ${HEADER_FILES}
    # "${CMAKE_CURRENT_SOURCE_DIR}/viewport_camera.hpp"
    PARENT_SCOPE
)

set(PRIVATE_INCLUDE_DIRECTORIES
    ${PRIVATE_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)
