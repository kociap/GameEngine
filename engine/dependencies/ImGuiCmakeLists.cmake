option(IMGUI_BUILD_SDL_OPENGL_EXAMPLES OFF)
option(IMGUI_BUILD_SDL_VULKAN_EXAMPLES OFF)
option(IMGUI_BUILD_GLFW_OPENGL_EXAMPLES OFF)
option(IMGUI_BUILD_GLFW_VULKAN_EXAMPLES OFF)
option(IMGUI_EXAMPLES_INCLUDE_MAIN OFF)

set(IMGUI_LIB_NAME "imgui")
set(IMGUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

add_library(${IMGUI_LIB_NAME})

set_target_properties(${IMGUI_LIB_NAME}
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}/${IMGUI_LIB_NAME}"
)

target_include_directories(${IMGUI_LIB_NAME} PUBLIC "${IMGUI_PATH}" "${IMGUI_PATH}/misc/cpp")
target_sources(${IMGUI_LIB_NAME}
    PRIVATE 
    "${IMGUI_PATH}/imgui.h"
    "${IMGUI_PATH}/imconfig.h"
    "${IMGUI_PATH}/imgui_internal.h"
    "${IMGUI_PATH}/imstb_rectpack.h"
    "${IMGUI_PATH}/imstb_textedit.h"
    "${IMGUI_PATH}/imstb_truetype.h"
    "${IMGUI_PATH}/imgui.cpp"
    "${IMGUI_PATH}/imgui_demo.cpp"
    "${IMGUI_PATH}/imgui_draw.cpp"
    "${IMGUI_PATH}/imgui_widgets.cpp"
    "${IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp"
    "${IMGUI_PATH}/misc/cpp/imgui_stdlib.h"
)

if(IMGUI_BUILD_GLFW_OPENGL_EXAMPLES)
    target_include_directories(${IMGUI_LIB_NAME} PUBLIC "${IMGUI_PATH}/examples")
    target_link_libraries(${IMGUI_LIB_NAME} glad glfw)
    target_sources(${IMGUI_LIB_NAME}
        PRIVATE
        "${IMGUI_PATH}/examples/imgui_impl_glfw.h"
        "${IMGUI_PATH}/examples/imgui_impl_glfw.cpp"
        "${IMGUI_PATH}/examples/imgui_impl_opengl3.h"
        "${IMGUI_PATH}/examples/imgui_impl_opengl3.cpp"
    )
endif()

if(IMGUI_BUILD_SDL_OPENGL_EXAMPLES)
    message(FATAL "SDL not yet supported")
    target_include_directories(${IMGUI_LIB_NAME} PUBLIC "${IMGUI_PATH}/examples")
    target_link_libraries(${IMGUI_LIB_NAME} glad)
    target_sources(${IMGUI_LIB_NAME}
        PRIVATE
        "${IMGUI_PATH}/examples/imgui_impl_sdl.h"
        "${IMGUI_PATH}/examples/imgui_impl_sdl.cpp"
        "${IMGUI_PATH}/examples/imgui_impl_opengl3.h"
        "${IMGUI_PATH}/examples/imgui_impl_opengl3.cpp"
    )
endif()

if(IMGUI_BUILD_SDL_VULKAN_EXAMPLES OR IMGUI_BUILD_GLFW_VULKAN_EXAMPLES)
    message(FATAL "Vulkan is not yet supported")
endif()
