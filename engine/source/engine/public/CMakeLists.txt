set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/directionallight.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/pointlight.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/spotlight.hpp"
    PARENT_SCOPE
)


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
