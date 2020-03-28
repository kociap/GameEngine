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
#include <core/exception.hpp>

#include <stdio.h>

#include <fstream>
#include <stdexcept>

namespace anton_engine::assets {
    static atl::String fs_path_to_string(std::filesystem::path const& path) {
        std::string gen_str = path.generic_string();
        return {gen_str.data(), (i64)gen_str.size()};
    }

    atl::String read_file_raw_string(std::filesystem::path const& _path) {
        atl::String path = fs_path_to_string(_path);
        FILE* const file = fopen(path.data(), "r");
        if (!file) {
            atl::String error_msg{u8"Could not open file "};
            error_msg.append(path);
            throw Exception(error_msg);
        }

        atl::String out;
        for(int c = fgetc(file); c != EOF; c = fgetc(file)) {
            out.append((char8)c);
        }
        fclose(file);
        return out;
    }

    opengl::Shader_Type shader_type_from_filename(std::filesystem::path const& filename) {
        atl::String const extension = fs_path_to_string(filename.extension());
        if (extension == u8".vert") {
            return opengl::Shader_Type::vertex_shader;
        } else if (extension == u8".frag") {
            return opengl::Shader_Type::fragment_shader;
        } else if (extension == u8".geom") {
            return opengl::Shader_Type::geometry_shader;
        } else if (extension == u8".comp") {
            return opengl::Shader_Type::compute_shader;
        } else if (extension == u8".tese") {
            return opengl::Shader_Type::tessellation_evaluation_shader;
        } else if (extension == u8".tesc") {
            return opengl::Shader_Type::tessellation_control_shader;
        } else {
            throw Exception(u8"\"" + extension + u8"\" is not a known shader file extension");
        }
    }

    Shader_File load_shader_file(std::filesystem::path const& _path) {
        // TODO: Will not compile in shipping build.
        std::filesystem::path full_path = utils::concat_paths(paths::shaders_directory(), _path);
        atl::String const path = fs_path_to_string(_path);
        atl::String const shader_source = read_file_raw_string(full_path);
        opengl::Shader_Type const type = shader_type_from_filename(_path);
        return Shader_File(path, type, shader_source);
    }

    static uint8_t read_uint8(FILE* stream) {
        return fgetc(stream);
    }

    static u16 read_uint16_le(FILE* stream) {
        return static_cast<u16>(read_uint8(stream)) | (static_cast<u16>(read_uint8(stream)) << 8);
    }

    static u32 read_uint32_le(FILE* stream) {
        return static_cast<u32>(read_uint16_le(stream)) | (static_cast<u32>(read_uint16_le(stream)) << 16);
    }

    static u64 read_uint64_le(FILE* stream) {
        return static_cast<u64>(read_uint32_le(stream)) | (static_cast<u64>(read_uint32_le(stream)) << 32);
    }

    static i32 read_int32_le(FILE* stream) {
        return (fgetc(stream)) | (fgetc(stream) << 8) | (fgetc(stream) << 16) | (fgetc(stream) << 24);
    }

    static i64 read_int64_le(FILE* stream) {
        return (static_cast<i64>(read_int32_le(stream)) | static_cast<i64>(read_int32_le(stream)) << 32);
    }

    // TODO extract writing and reading to one tu to keep them in sync
    Texture_Format load_texture_no_mipmaps(std::string const& filename, u64 const texture_id, atl::Vector<uint8_t>& pixels) {
#if !GE_BUILD_SHIPPING
        std::filesystem::path const texture_path = utils::concat_paths(paths::assets_directory(), filename + ".getex");
        // TODO texture loading
        atl::String path = fs_path_to_string(texture_path);
        FILE* file = fopen(path.data(), "rb");
        while (file && !feof(file)) {
            [[maybe_unused]] i64 const texture_data_size = read_int64_le(file);
            u64 const tex_id = read_uint64_le(file);
            if (tex_id == texture_id) {
                Texture_Format format;
                fread(reinterpret_cast<char*>(&format), sizeof(Texture_Format), 1, file);
                i64 texture_size_bytes = read_int64_le(file);
                pixels.resize(texture_size_bytes);
                fread(reinterpret_cast<char*>(pixels.data()), texture_size_bytes, 1, file);
                fclose(file);
                return format;
            } else {
                // Since there's only one texture per file right now, we do not have to skip past it, but instead throw an exception
                throw Exception("Texture not found in the file " + fs_path_to_string(texture_path));
            }
        }
        throw Exception("Invalid texture file");
#else
#    error "No implementation of load_texture_no_mipmaps for shipping build."
// TODO shipping build texture loading
#endif
    }

    Mesh load_mesh(std::filesystem::path const& filename, u64 const guid) {
        // TODO: Shipping build. Files will be packed.
        auto asset_path = utils::concat_paths(paths::assets_directory(), filename);
        asset_path.replace_extension(".mesh");
        atl::String path = fs_path_to_string(asset_path);
        FILE* file = fopen(path.data(), "rb");
        while (file) {
            u64 const extracted_guid = read_uint64_le(file);
            if (extracted_guid != guid) {
                i64 const vertex_count = read_int64_le(file);
                fseek(file, vertex_count * sizeof(Vertex), SEEK_CUR);
                i64 const index_count = read_int64_le(file);
                fseek(file, index_count * sizeof(u32), SEEK_CUR);
            } else {
                i64 const vertex_count = read_int64_le(file);
                atl::Vector<Vertex> vertices(vertex_count);
                fread(reinterpret_cast<char*>(vertices.data()), vertex_count * sizeof(Vertex), 1, file);
                i64 const index_count = read_int64_le(file);
                atl::Vector<u32> indices(index_count);
                fread(reinterpret_cast<char*>(indices.data()), index_count * sizeof(u32), 1, file);
                fclose(file);
                return {atl::move(vertices), atl::move(indices)};
            }
        }

        // TODO: Fix this very primitive error handling.
        throw Exception("Mesh not found");
    }
} // namespace anton_engine::assets
