#pragma once

#include <anton/array.hpp>
#include <anton/string_view.hpp>
#include <core/types.hpp>
#include <rendering/texture_format.hpp>
#include <shaders/shader_stage.hpp>

namespace anton_engine {
  class Mesh;

  namespace assets {
    // Reads file as a string of chars without interpreting it
    anton::String read_file_raw_string(anton::String_View filename);

    opengl::Shader_Stage_Type
    shader_stage_type_from_filename(anton::String_View filename);

    Shader_Stage load_shader_stage(anton::String_View path);

    // Loads texture pixels
    Texture_Format load_texture_no_mipmaps(anton::String_View filename,
                                           u64 texture_id,
                                           anton::Array<u8>& pixels);

    Mesh load_mesh(anton::String_View filename, u64 guid);
  } // namespace assets
} // namespace anton_engine
