cmake_minimum_required(VERSION 3.12)

project(Codegen CXX)

add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/codegen/component_serialization_generator.cpp")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
    FOLDER ${ENGINE_TOOLS_FOLDER}
)

target_compile_options(Codegen PRIVATE ${ENGINE_CXX_FLAGS})

add_subdirectory(codegen/private)
add_subdirectory(codegen/public)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../source/public")
target_sources(${PROJECT_NAME} 
    PRIVATE ${SOURCE_FILES} 
    PRIVATE ${HEADER_FILES}
)

target_link_libraries(Codegen
    anton_engine
)

target_compile_definitions(Codegen 
    PRIVATE 
    ENGINE_API=${ENGINE_DLL_IMPORT} 
    ANTON_COMPILER_ID=${ENGINE_COMPILER_ID} 
    ANTON_COMPILER_CLANG=${ENGINE_COMPILER_CLANG} 
    ANTON_COMPILER_GCC=${ENGINE_COMPILER_GCC} 
    ANTON_COMPILER_MSVC=${ENGINE_COMPILER_MSVC} 
    ANTON_COMPILER_UNKNOWN=${ENGINE_COMPILER_UNKNOWN}
    ANTON_WITH_EDITOR=${ENGINE_WITH_EDITOR}
    # Use unicode instead of multibyte charset (VS)
    UNICODE
    _UNICODE
)
