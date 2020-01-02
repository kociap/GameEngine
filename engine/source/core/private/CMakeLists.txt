if(WIN32) # TODO other platforms
    set(PLATFORM_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/anton_assert/anton_assert_win.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/common.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/debugging.cpp"
    )

    set(PLATFORM_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/common.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/windows/debugging.hpp"
    )
else()
    set(PLATROFM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/anton_assert/anton_assert_posix.cpp"
    )
endif()

set(SOURCE_FILES ${SOURCE_FILES} ${PLATFORM_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/anton_stl/allocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/anton_stl/string_iterators.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/anton_stl/string.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serialization/archives/binary.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/unicode/common.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/simple_xml_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_macros.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/logging.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/paths.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random.cpp"
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
