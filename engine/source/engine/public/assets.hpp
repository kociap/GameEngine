#ifndef ENGINE_ASSETS_ASSETS_HPP_INCLUDE
#define ENGINE_ASSETS_ASSETS_HPP_INCLUDE

#include "shaderfile.hpp"

#include <filesystem>
#include <vector>

class Mesh;

class Assets {
public:
    static void init(std::filesystem::path current_path, std::filesystem::path assets_path, std::filesystem::path shaders_path);

    // Gets the path in which the executable is located
    static std::filesystem::path current_path();

    // Reads file as a string of chars without interpreting it
    static void read_file_raw(std::filesystem::path const& filename, std::string& out);

    static Shader_type shader_type_from_filename(std::filesystem::path const& filename);

    static Shader_File load_shader_file(std::filesystem::path const& path);

    // Load shader file, compile it and attach to a shader
    static void load_shader_file_and_attach(Shader& shader, std::filesystem::path const& path);

    // Loads cubemap and returns a OpenGL handle
    static uint32_t load_cubemap(std::vector<std::filesystem::path> const& paths);

    // Loads texture and returns a OpenGL handle
    static uint32_t load_texture(std::filesystem::path filename);

	static std::vector<Mesh> load_model(std::filesystem::path const& filename);

private:
    static std::filesystem::path _current_path;
    static std::filesystem::path _assets_path;
    static std::filesystem::path _shaders_path;
};

#endif // !ENGINE_ASSETS_ASSETS_HPP_INCLUDE