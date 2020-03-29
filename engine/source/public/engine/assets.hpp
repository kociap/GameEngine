#ifndef ENGINE_ASSETS_ASSETS_HPP_INCLUDE
#define ENGINE_ASSETS_ASSETS_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/string_view.hpp>
#include <core/atl/vector.hpp>
#include <shaders/shader_file.hpp>
#include <rendering/texture_format.hpp>

namespace anton_engine {
    class Mesh;

    namespace assets {
        // Reads file as a string of chars without interpreting it
        atl::String read_file_raw_string(atl::String_View filename);

        opengl::Shader_Type shader_type_from_filename(atl::String_View filename);

        Shader_File load_shader_file(atl::String_View path);

        // Loads texture pixels
        Texture_Format load_texture_no_mipmaps(atl::String_View filename, u64 texture_id, atl::Vector<u8>& pixels);

        Mesh load_mesh(atl::String_View filename, u64 guid);
    } // namespace assets
} // namespace anton_engine

#endif // !ENGINE_ASSETS_ASSETS_HPP_INCLUDE
