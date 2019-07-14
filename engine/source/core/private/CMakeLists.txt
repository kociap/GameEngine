set(SOURCE_FILES ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector4.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix4.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/quaternion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/math.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/simple_xml_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_macros.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/paths.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/paths_internal.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_DIR}") # TODO shouldn't be public