#include <engine/assets.hpp>

#include <rendering/glad.hpp>

#include <anton/array.hpp>
#include <anton/filesystem.hpp>
#include <anton/math/vec3.hpp>
#include <build_config.hpp>
#include <core/exception.hpp>
#include <core/paths.hpp>
#include <core/types.hpp>
#include <engine/mesh.hpp>
#include <rendering/opengl.hpp>
#include <rendering/texture_format.hpp>
#include <shaders/shader.hpp>

namespace anton_engine::assets {
  anton::String read_file_raw_string(anton::String_View const path)
  {
    FILE* const file = fopen(path.data(), "r");
    if(!file) {
      anton::String error_msg{u8"Could not open file "};
      error_msg.append(path);
      throw Exception(error_msg);
    }

    anton::String out;
    for(int c = fgetc(file); c != EOF; c = fgetc(file)) {
      out.append((char8)c);
    }
    fclose(file);
    return out;
  }

  opengl::Shader_Stage_Type
  shader_stage_type_from_filename(anton::String_View const filename)
  {
    anton::String_View const ext = anton::fs::get_extension(filename);
    if(ext == u8".vert") {
      return opengl::Shader_Stage_Type::vertex_shader;
    } else if(ext == u8".frag") {
      return opengl::Shader_Stage_Type::fragment_shader;
    } else if(ext == u8".geom") {
      return opengl::Shader_Stage_Type::geometry_shader;
    } else if(ext == u8".comp") {
      return opengl::Shader_Stage_Type::compute_shader;
    } else if(ext == u8".tese") {
      return opengl::Shader_Stage_Type::tessellation_evaluation_shader;
    } else if(ext == u8".tesc") {
      return opengl::Shader_Stage_Type::tessellation_control_shader;
    } else {
      throw Exception(
        anton::concat("\"", ext, "\" is not a known shader file extension"));
    }
  }

  Shader_Stage load_shader_stage(anton::String_View const path)
  {
    // TODO: Will not compile in shipping build.
    anton::String const full_path =
      anton::fs::concat_paths(paths::shaders_directory(), path);
    anton::String const shader_source = read_file_raw_string(full_path);
    opengl::Shader_Stage_Type const type =
      shader_stage_type_from_filename(path);
    return Shader_Stage(path, type, shader_source);
  }

  static u8 read_uint8(FILE* stream)
  {
    return fgetc(stream);
  }

  static u16 read_uint16_le(FILE* stream)
  {
    return static_cast<u16>(read_uint8(stream)) |
           (static_cast<u16>(read_uint8(stream)) << 8);
  }

  static u32 read_uint32_le(FILE* stream)
  {
    return static_cast<u32>(read_uint16_le(stream)) |
           (static_cast<u32>(read_uint16_le(stream)) << 16);
  }

  static u64 read_uint64_le(FILE* stream)
  {
    return static_cast<u64>(read_uint32_le(stream)) |
           (static_cast<u64>(read_uint32_le(stream)) << 32);
  }

  static i32 read_int32_le(FILE* stream)
  {
    return (fgetc(stream)) | (fgetc(stream) << 8) | (fgetc(stream) << 16) |
           (fgetc(stream) << 24);
  }

  static i64 read_int64_le(FILE* stream)
  {
    return (static_cast<i64>(read_int32_le(stream)) |
            static_cast<i64>(read_int32_le(stream)) << 32);
  }

  // TODO extract writing and reading to one tu to keep them in sync
  Texture_Format load_texture_no_mipmaps(anton::String_View const filename,
                                         u64 const texture_id,
                                         anton::Array<u8>& pixels)
  {
#if !GE_BUILD_SHIPPING
    anton::String const filename_ext = anton::String(filename) + ".getex";
    anton::String const texture_path =
      anton::fs::concat_paths(paths::assets_directory(), filename_ext);
    // TODO texture loading
    FILE* file = fopen(texture_path.data(), "rb");
    while(file && !feof(file)) {
      [[maybe_unused]] i64 const texture_data_size = read_int64_le(file);
      u64 const tex_id = read_uint64_le(file);
      if(tex_id == texture_id) {
        Texture_Format format;
        fread(reinterpret_cast<char*>(&format), sizeof(Texture_Format), 1,
              file);
        i64 const texture_size_bytes = read_int64_le(file);
        pixels.resize(texture_size_bytes);
        fread(reinterpret_cast<char*>(pixels.data()), texture_size_bytes, 1,
              file);
        fclose(file);
        return format;
      } else {
        // Since there's only one texture per file right now, we do not have to skip past it, but instead throw an exception
        throw Exception(u8"Texture not found in the file " + texture_path);
      }
    }
    throw Exception(u8"Invalid texture file");
#else
  #error "No implementation of load_texture_no_mipmaps for shipping build."
// TODO shipping build texture loading
#endif
  }

  Mesh load_mesh(anton::String_View const filename, u64 const guid)
  {
    // TODO: Shipping build. Files will be packed.
    anton::String_View const filename_no_ext =
      anton::fs::remove_extension(filename);
    anton::String asset_path =
      anton::fs::concat_paths(paths::assets_directory(), filename_no_ext);
    asset_path.append(u8".mesh");
    FILE* file = fopen(asset_path.data(), "rb");
    while(file) {
      u64 const extracted_guid = read_uint64_le(file);
      if(extracted_guid != guid) {
        i64 const vertex_count = read_int64_le(file);
        fseek(file, vertex_count * sizeof(Vertex), SEEK_CUR);
        i64 const index_count = read_int64_le(file);
        fseek(file, index_count * sizeof(u32), SEEK_CUR);
      } else {
        i64 const vertex_count = read_int64_le(file);
        anton::Array<Vertex> vertices(vertex_count);
        fread(reinterpret_cast<char*>(vertices.data()),
              vertex_count * sizeof(Vertex), 1, file);
        i64 const index_count = read_int64_le(file);
        anton::Array<u32> indices(index_count);
        fread(reinterpret_cast<char*>(indices.data()),
              index_count * sizeof(u32), 1, file);
        fclose(file);
        return {ANTON_MOV(vertices), ANTON_MOV(indices)};
      }
    }

    // TODO: Fix this very primitive error handling.
    throw Exception(u8"Mesh not found");
  }
} // namespace anton_engine::assets
