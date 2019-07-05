cmake_minimum_required(VERSION 3.12)

project(Codegen CXX)

add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/codegen/component_serialization_generator.cpp")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
    FOLDER ${ENGINE_TOOLS_FOLDER}
)

add_subdirectory(codegen/private)
add_subdirectory(codegen/public)

target_include_directories(${PROJECT_NAME} PRIVATE ${ENGINE_INCLUDE_DIRECTORIES})
target_sources(${PROJECT_NAME} 
    PRIVATE ${SOURCE_FILES} 
    PRIVATE ${HEADER_FILES}
)