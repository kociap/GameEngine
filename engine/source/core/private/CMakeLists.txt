if(WIN32) # TODO other platforms
    set(PLATFORM_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/assert/assert_win.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/common.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/debugging.cpp"
    )

    set(PLATFORM_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/common.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/debugging.hpp"
    )
else()
    set(PLATROFM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/assert/assert_posix.cpp"
    )
endif()

set(SOURCE_FILES ${SOURCE_FILES} ${PLATFORM_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/anton_stl/allocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/anton_stl/string_iterators.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/anton_stl/string_view.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/anton_stl/string.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector4.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix4.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/quaternion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/math.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serialization/archives/binary.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/unicode/common.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/simple_xml_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_macros.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/paths.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES ${HEADER_FILES} ${PLATFORM_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/paths_internal.hpp"
    PARENT_SCOPE
)

set(PRIVATE_INCLUDE_DIRECTORIES
    ${PRIVATE_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)
