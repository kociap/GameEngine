add_subdirectory(freetype)
add_subdirectory(glad)
add_subdirectory(zlib)
add_subdirectory(mimas)

set_target_properties(glad zlib freetype mimas
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}"
    POSITION_INDEPENDENT_CODE TRUE
    COMPILE_OPTIONS ${ENGINE_CXX_DEPENDENCIES_FLAGS}
)
