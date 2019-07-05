#ifndef ENGINE_ASSETS_ASSETS_HPP_INCLUDE
#define ENGINE_ASSETS_ASSETS_HPP_INCLUDE

#include <containers/vector.hpp>
#include <shader_file.hpp>

#include <filesystem>

class Mesh;

namespace assets {
    void init(std::filesystem::path current_path, std::filesystem::path assets_path, std::filesystem::path shaders_path);

    // Gets the path in which the executable is located
    std::filesystem::path current_path();

    // Reads file as a string of chars without interpreting it
    std::string read_file_raw_string(std::filesystem::path const& filename);
    containers::Vector<uint8_t> read_file_raw(std::filesystem::path const& filename);

    opengl::Shader_Type shader_type_from_filename(std::filesystem::path const& filename);

    Shader_File load_shader_file(std::filesystem::path const& path);

    // Loads cubemap and returns a OpenGL handle
    uint32_t load_cubemap(std::vector<std::filesystem::path> const& paths);

    // Loads texture and returns a OpenGL handle
    uint32_t load_texture(std::filesystem::path filename, bool flip_image);
    uint32_t load_srgb_texture(std::filesystem::path filename, bool flip_image);

    containers::Vector<Mesh> load_model(std::filesystem::path const& filename);
} // namespace assets

#endif // !ENGINE_ASSETS_ASSETS_HPP_INCLUDE