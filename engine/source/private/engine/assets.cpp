#include <engine/assets.hpp>

#include <rendering/glad.hpp>

#include <core/types.hpp>
#include <core/atl/vector.hpp>
#include <build_config.hpp>
#include <core/math/vector3.hpp>
#include <engine/mesh.hpp>
#include <rendering/opengl.hpp>
#include <core/paths.hpp>
#include <shaders/shader.hpp>
#include <rendering/texture_format.hpp>
#include <core/utils/filesystem.hpp>

#include <fstream>
#include <stdexcept>

namespace anton_engine::assets {
    std::string read_file_raw_string(std::filesystem::path const& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::invalid_argument("Could not open file " + filename.generic_string());
        }
        std::string out;
        std::getline(file, out, '\0');
        return out;
    }

    opengl::Shader_Type shader_type_from_filename(std::filesystem::path const& filename) {
        std::string extension(filename.extension().generic_string());
        if (extension == ".vert") {
            return opengl::Shader_Type::vertex_shader;
        } else if (extension == ".frag") {
            return opengl::Shader_Type::fragment_shader;
        } else if (extension == ".geom") {
            return opengl::Shader_Type::geometry_shader;
        } else if (extension == ".comp") {
            return opengl::Shader_Type::compute_shader;
        } else if (extension == ".tese") {
            return opengl::Shader_Type::tessellation_evaluation_shader;
        } else if (extension == ".tesc") {
            return opengl::Shader_Type::tessellation_control_shader;
        } else {
            throw std::invalid_argument("\"" + extension + "\" is not a known shader file extension");
        }
    }

    Shader_File load_shader_file(std::filesystem::path const& path) {
        // TODO will not compile in shipping build
        std::filesystem::path full_path = utils::concat_paths(paths::shaders_directory(), path);
        std::string shader_source = read_file_raw_string(full_path);
        std::string path_as_str = path.generic_string();
        return Shader_File(path_as_str.data(), shader_type_from_filename(path), shader_source.data());
    }

    static uint8_t read_uint8(std::ifstream& stream) {
        return stream.get();
    }

    static uint16_t read_uint16_le(std::ifstream& stream) {
        return static_cast<uint16_t>(read_uint8(stream)) | (static_cast<uint16_t>(read_uint8(stream)) << 8);
    }

    static uint32_t read_uint32_le(std::ifstream& stream) {
        return static_cast<uint32_t>(read_uint16_le(stream)) | (static_cast<uint32_t>(read_uint16_le(stream)) << 16);
    }

    static uint64_t read_uint64_le(std::ifstream& stream) {
        return static_cast<uint64_t>(read_uint32_le(stream)) | (static_cast<uint64_t>(read_uint32_le(stream)) << 32);
    }

    static int32_t read_int32_le(std::ifstream& stream) {
        return (stream.get()) | (stream.get() << 8) | (stream.get() << 16) | (stream.get() << 24);
    }

    static int64_t read_int64_le(std::ifstream& stream) {
        return (static_cast<int64_t>(read_int32_le(stream)) | static_cast<int64_t>(read_int32_le(stream)) << 32);
    }

    // TODO extract writing and reading to one tu to keep them in sync
    Texture_Format load_texture_no_mipmaps(std::string const& filename, uint64_t const texture_id, atl::Vector<uint8_t>& pixels) {
#if !GE_BUILD_SHIPPING
        std::filesystem::path const texture_path = utils::concat_paths(paths::assets_directory(), filename + ".getex");
        // TODO texture loading
        std::ifstream file(texture_path, std::ios::binary);
        while (!file.eof()) {
            [[maybe_unused]] int64_t const texture_data_size = read_int64_le(file);
            uint64_t const tex_id = read_uint64_le(file);
            if (tex_id == texture_id) {
                Texture_Format format;
                file.read(reinterpret_cast<char*>(&format), sizeof(Texture_Format));
                int64_t texture_size_bytes = read_int64_le(file);
                pixels.resize(texture_size_bytes);
                file.read(reinterpret_cast<char*>(pixels.data()), texture_size_bytes);
                return format;
            } else {
                // Since there's only one texture per file right now, we do not have to skip past it, but instead throw an exception
                throw std::runtime_error("Texture not found in the file " + texture_path.generic_string());
            }
        }
        throw std::runtime_error("Invalid texture file");
#else
#    error "No implementation of load_texture_no_mipmaps for shipping build."
// TODO shipping build texture loading
#endif
    }

    Mesh load_mesh(std::filesystem::path const& filename, u64 const guid) {
        // TODO: Shipping build. Files will be packed.
        auto asset_path = utils::concat_paths(paths::assets_directory(), filename);
        asset_path.replace_extension(".mesh");
        std::ifstream file(asset_path, std::ios::in | std::ios::binary);
        while (file) {
            u64 extracted_guid;
            file.read(reinterpret_cast<char*>(&extracted_guid), sizeof(u64));
            if (extracted_guid != guid) {
                i64 vertex_count;
                file.read(reinterpret_cast<char*>(&vertex_count), sizeof(i64));
                file.seekg(vertex_count * sizeof(Vertex), std::ios::cur);
                i64 index_count;
                file.read(reinterpret_cast<char*>(&index_count), sizeof(i64));
                file.seekg(index_count * sizeof(u32), std::ios::cur);
            } else {
                i64 vertex_count;
                file.read(reinterpret_cast<char*>(&vertex_count), sizeof(i64));
                atl::Vector<Vertex> vertices(vertex_count);
                file.read(reinterpret_cast<char*>(vertices.data()), vertex_count * sizeof(Vertex));
                i64 index_count;
                file.read(reinterpret_cast<char*>(&index_count), sizeof(i64));
                atl::Vector<u32> indices(index_count);
                file.read(reinterpret_cast<char*>(indices.data()), index_count * sizeof(u32));
                return {atl::move(vertices), atl::move(indices)};
            }
        }

        // TODO: Fix this very primitive error handling.
        throw std::runtime_error("Mesh not found");
    }
} // namespace anton_engine::assets
