set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/model.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/object.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/game_object.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/point_light_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/static_mesh_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/renderable_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/spot_light_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/directional_light_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/base_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/camera.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/object_manager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/guid.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/transform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/plane.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/cube.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/renderer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/framebuffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/uniform_buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/framebuffer_construct_info.hpp"
    PARENT_SCOPE
)


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
