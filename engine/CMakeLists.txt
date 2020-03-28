find_package(OpenGL REQUIRED)

# Debug Interface Access. Might use it one day for better assert
# if(WIN32) 
#     find_path(DIA_SDK_DIRECTORY
#         NAMES "include/dia2.h"
#         HINTS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/DIA SDK"
#               "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/DIA SDK"
#     )

#     set(DIA_SDK_INCLUDE_DIRECTORY "${DIA_SDK_DIRECTORY}/include")
# endif()

add_subdirectory(dependencies)
add_subdirectory(source)
add_subdirectory(shaders)
if(${ENGINE_WITH_EDITOR})
    add_subdirectory(editor)
endif()

if(${ANTON_BUILD_TOOLS})
    add_subdirectory(tools)
endif()

set_target_properties(anton_engine anton_engine_loader
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${ENGINE_BINARY_OUTPUT_DIRECTORY}"
)
