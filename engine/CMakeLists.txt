# TODO: scripts is a temporary directory to store a bunch of game scripts that e.g. move the camera around. Should probably remove soon.

add_library(anton_engine)

target_compile_definitions(anton_engine
  PRIVATE
  ENGINE_API=${ENGINE_DLL_EXPORT}
  GAME_API=${ENGINE_DLL_IMPORT}
  ANTON_WITH_EDITOR=$<BOOL:${ENGINE_BUILD_EDITOR}>
  DESERIALIZE=$<BOOL:${DESERIALIZE}>
  # Use unicode instead of multibyte charset (VS)
  UNICODE
  _UNICODE
  _CRT_SECURE_NO_WARNINGS
)

if(ENGINE_COMPILER_CLANG)
  target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_CLANG)
endif()

if(ENGINE_COMPILER_GCC)
  target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_GCC)
endif()

if(ENGINE_COMPILER_MSVC)
  target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_MSVC)
endif()

if(ENGINE_COMPILER_UNKNOWN)
  target_compile_definitions(anton_engine PRIVATE ANTON_COMPILER_UNKNOWN)
endif()

target_compile_options(anton_engine PRIVATE ${ANTON_COMPILE_FLAGS})

target_include_directories(anton_engine
  PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/public"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

target_sources(anton_engine
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/paths.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/json.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/utils/filesystem.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/random.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/threads.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/logging.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/serialization/archives/binary.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/core/paths_internal.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/shaders/shader_stage.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/shaders/shader_exceptions.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/shaders/shader.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/shaders/builtin_shaders.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/windowing/window.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/windowing/window.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/physics/obb.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/physics/ray.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/physics/intersection_tests.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/physics/line.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/time_internal.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/assets.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/mesh.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/input/input_internal.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/input/input.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/time.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/ecs/jobs_management.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/ecs/jobs.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/ecs/system.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/ecs/ecs.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/ecs/component_serialization.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/ecs/system_management.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/ecs/jobs_management.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/engine/key.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/scripts/debug_hotkeys.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/scripts/camera_movement.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/rendering/texture_format.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/rendering/opengl.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/rendering/fonts.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/rendering/renderer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/private/rendering/framebuffer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/handle.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/types.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/class_macros.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/diagnostic_macros.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/utils/filesystem.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/utils/enum.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/random.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/logging.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/serialization/archives/binary.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/serialization/types/array.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/serialization/types/string.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/serialization/serialization.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/exception.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/integer_sequence_generator.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/paths.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/memory/stack_allocate.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/threads.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/json.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/core/color.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/shaders/shader.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/shaders/builtin_shaders.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/shaders/shader_exceptions.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/shaders/shader_stage.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/physics/line.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/physics/obb.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/physics/intersections_common.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/physics/intersection_tests.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/physics/ray.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/module_loader.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/key.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/input.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/material.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/spot_light_component.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/point_light_component.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/transform.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/camera.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/ortographic_camera.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/directional_light_component.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/static_mesh_component.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/entity_name.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/line_component.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/components/hierarchy.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/mesh.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/time.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/assets.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/component_view.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/jobs.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/entity.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/component_serialization.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/ecs.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/system.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/component_container.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/ecs/component_container_iterator.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/engine/resource_manager.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/module_loader.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/scripts/camera_movement.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/scripts/debug_hotkeys.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/rendering/fonts.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/rendering/opengl_enums_defs.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/rendering/framebuffer.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/rendering/glad.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/rendering/renderer.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/rendering/opengl_enums_undefs.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/rendering/opengl.hpp"
)

if(UNIX)
  set(ENGINE_LINK_LIBS ${ENGINE_LINK_LIBS} dl)
elseif(WIN32)
  set(ENGINE_LINK_LIBS ${ENGINE_LINK_LIBS} DbgHelp)

  target_sources(anton_engine
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/common.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/debugging.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/debugging.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/core/windows/common.hpp"
  )
endif()

target_link_libraries(anton_engine
  ${ENGINE_LINK_LIBS}
  glad
  zlib
  freetype
  mimas
  anton_core
)

add_executable(anton_engine_loader "${CMAKE_CURRENT_SOURCE_DIR}/public/main.cpp")

target_compile_definitions(anton_engine_loader
  PRIVATE
  ENGINE_API=${ENGINE_DLL_IMPORT}
  GAME_API=${ENGINE_DLL_IMPORT}
  ANTON_WITH_EDITOR=$<BOOL:${ENGINE_BUILD_EDITOR}>
  UNICODE
  _UNICODE
  _CRT_SECURE_NO_WARNINGS
)

if(ENGINE_COMPILER_CLANG)
  target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_CLANG)
endif()

if(ENGINE_COMPILER_GCC)
  target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_GCC)
endif()

if(ENGINE_COMPILER_MSVC)
  target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_MSVC)
endif()

if(ENGINE_COMPILER_UNKNOWN)
  target_compile_definitions(anton_engine_loader PRIVATE ANTON_COMPILER_UNKNOWN)
endif()

target_compile_options(anton_engine_loader PRIVATE ${ANTON_COMPILE_FLAGS})

target_include_directories(anton_engine_loader
  PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/public"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

target_link_libraries(anton_engine_loader anton_engine)

target_link_options(anton_engine_loader PRIVATE ${ANTON_LINK_FLAGS})
target_link_options(anton_engine PRIVATE ${ANTON_LINK_FLAGS})
