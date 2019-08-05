#ifndef ENGINE_ASSETS_ASSETS_HPP_INCLUDE
#define ENGINE_ASSETS_ASSETS_HPP_INCLUDE

#include <containers/vector.hpp>
#include <shader_file.hpp>

#include <filesystem>
#include <string>

class Mesh;

namespace assets {
    // Reads file as a string of chars without interpreting it
    std::string read_file_raw_string(std::filesystem::path const& filename);

    opengl::Shader_Type shader_type_from_filename(std::filesystem::path const& filename);

    Shader_File load_shader_file(std::filesystem::path const& path);

    // Loads texture and returns a OpenGL handle
    uint32_t load_texture(std::string const& filename, uint64_t texture_id);

    containers::Vector<Mesh> load_model(std::filesystem::path const& filename);
} // namespace assets

#endif // !ENGINE_ASSETS_ASSETS_HPP_INCLUDE