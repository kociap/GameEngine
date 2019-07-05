set(SOURCE_FILES
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/decoders/png.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/decoders/png.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")