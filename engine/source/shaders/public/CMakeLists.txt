set(HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exceptions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/program.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/uniform.hpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")