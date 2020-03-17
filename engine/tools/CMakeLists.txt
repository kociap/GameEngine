cmake_minimum_required(VERSION 3.13)

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
    ANTON_WITH_EDITOR=${ENGINE_WITH_EDITOR}
    # Use unicode instead of multibyte charset (VS)
    UNICODE
    _UNICODE
)

if(ENGINE_COMPILER_CLANG)
    target_compile_definitions(Codegen PRIVATE ANTON_COMPILER_CLANG)
endif()

if(ENGINE_COMPILER_GCC)
    target_compile_definitions(Codegen PRIVATE ANTON_COMPILER_GCC)
endif()

if(ENGINE_COMPILER_MSVC)
    target_compile_definitions(Codegen PRIVATE ANTON_COMPILER_MSVC)
endif()

if(ENGINE_COMPILER_UNKNOWN)
    target_compile_definitions(Codegen PRIVATE ANTON_COMPILER_UNKNOWN)
endif()
