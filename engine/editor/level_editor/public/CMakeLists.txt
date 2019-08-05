set(HEADER_FILES
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/viewport.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/gizmo.hpp"
    PARENT_SCOPE
)

set(UI_FILES
    ${UI_FILES}
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")