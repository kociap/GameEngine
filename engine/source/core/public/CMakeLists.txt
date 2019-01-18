set(HEADER_FILES ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/math/math.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/quaternion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/transform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_macros.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/handle.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")