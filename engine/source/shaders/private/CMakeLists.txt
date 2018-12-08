set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exceptions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/program.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/uniform.cpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})