#pragma once

#include <core/color.hpp>
#include <core/types.hpp>
#include <core/utils/enum.hpp>
#include <rendering/opengl_enums_defs.hpp>

namespace anton_engine::opengl {
  enum class Sized_Internal_Format : u32 {
    // unsigned normalized
    r8 = GL_R8,
    r16 = GL_R16,
    rg8 = GL_RG8,
    rg16 = GL_RG16,
    rgb4 = GL_RGB4,
    rgb8 = GL_RGB8,
    rgb16 = GL_RGB16,
    rgba2 = GL_RGBA2,
    rgba4 = GL_RGBA4,
    rgba8 = GL_RGBA8,
    rgba16 = GL_RGBA16,
    // signed normalized
    r8_snorm = GL_R8_SNORM,
    r16_snorm = GL_R16_SNORM,
    rg8_snorm = GL_RG8_SNORM,
    rg16_snorm = GL_RG16_SNORM,
    rgb8_snorm = GL_RGB8_SNORM,
    rgb16_snorm = GL_RGB16_SNORM,
    rgba16_snorm = GL_RGBA16_SNORM,
    // float
    r16f = GL_R16F,
    rg16f = GL_RG16F,
    rgb16f = GL_RGB16F,
    rgba16f = GL_RGBA16F,
    r32f = GL_R32F,
    rg32f = GL_RG32F,
    rgb32f = GL_RGB32F,
    rgba32f = GL_RGBA32F,
    // signed integer
    r8i = GL_R8I,
    r16i = GL_R16I,
    r32i = GL_R32I,
    rg8i = GL_RG8I,
    rg16i = GL_RG16I,
    rg32i = GL_RG32I,
    rgb8i = GL_RGB8I,
    rgb16i = GL_RGB16I,
    rgb32i = GL_RGB32I,
    rgba16i = GL_RGBA16I,
    rgba32i = GL_RGBA32I,
    // unsigned integer
    r8ui = GL_R8UI,
    r16ui = GL_R16UI,
    r32ui = GL_R32UI,
    rg8ui = GL_RG8UI,
    rg16ui = GL_RG16UI,
    rg32ui = GL_RG32UI,
    rgb8ui = GL_RGB8UI,
    rgb16ui = GL_RGB16UI,
    rgb32ui = GL_RGB32UI,
    rgba16ui = GL_RGBA16UI,
    rgba32ui = GL_RGBA32UI,
    // SRGB
    srgb8 = GL_SRGB8,
    srgb8_alpha8 = GL_SRGB8_ALPHA8,
    // Depth and Stencil
    depth_component16 = GL_DEPTH_COMPONENT16,
    depth_component24 = GL_DEPTH_COMPONENT24,
    depth_component32 = GL_DEPTH_COMPONENT32, // Not required
    depth_component32f = GL_DEPTH_COMPONENT32F,
    depth24_stencil8 = GL_DEPTH24_STENCIL8,
    depth32f_stencil8 = GL_DEPTH32F_STENCIL8,
    stencil_index1 = GL_STENCIL_INDEX1, // Not required
    stencil_index4 = GL_STENCIL_INDEX4, // Not required
    stencil_index8 = GL_STENCIL_INDEX8,
    stencil_index16 = GL_STENCIL_INDEX16, // Not required
  };

  enum class Compressed_Internal_Format : u32 {
    // TODO compressed
  };

  enum class Format : u32 {
    stencil_index = GL_STENCIL_INDEX,
    depth_component = GL_DEPTH_COMPONENT,
    depth_stencil = GL_DEPTH_STENCIL,
    red = GL_RED,
    green = GL_GREEN,
    blue = GL_BLUE,
    rg = GL_RG,
    rgb = GL_RGB,
    rgba = GL_RGBA,
    bgr = GL_BGR,
    bgra = GL_BGRA,
    red_integer = GL_RED_INTEGER,
    green_integer = GL_GREEN_INTEGER,
    blue_integer = GL_BLUE_INTEGER,
    rg_integer = GL_RG_INTEGER,
    rgb_integer = GL_RGB_INTEGER,
    rgba_integer = GL_RGBA_INTEGER,
    bgr_integer = GL_BGR_INTEGER,
    bgra_integer = GL_BGRA_INTEGER,
  };

  enum class Type : u32 {
    unsigned_byte = GL_UNSIGNED_BYTE,
    signed_byte = GL_BYTE,
    unsigned_short = GL_UNSIGNED_SHORT,
    signed_short = GL_SHORT,
    unsigned_int = GL_UNSIGNED_INT,
    signed_int = GL_INT,
    half_float = GL_HALF_FLOAT,
    signed_float = GL_FLOAT,
  };

  enum class Attachment : u32 {
    color_attachment_0 = GL_COLOR_ATTACHMENT0,
    depth_attachment = GL_DEPTH_ATTACHMENT,
    depth_stencil_attachment = GL_DEPTH_STENCIL_ATTACHMENT,
    stencil_attachment = GL_STENCIL_ATTACHMENT,
  };

  enum class Buffer_Mask : u32 {
    color_buffer_bit = GL_COLOR_BUFFER_BIT,
    depth_buffer_bit = GL_DEPTH_BUFFER_BIT,
    stencil_buffer_bit = GL_STENCIL_BUFFER_BIT,
  };

  constexpr Buffer_Mask color_buffer_bit = Buffer_Mask::color_buffer_bit;
  constexpr Buffer_Mask depth_buffer_bit = Buffer_Mask::depth_buffer_bit;
  constexpr Buffer_Mask stencil_buffer_bit = Buffer_Mask::stencil_buffer_bit;

  enum class Shader_Stage_Type : u32 {
    vertex_shader = GL_VERTEX_SHADER,
    fragment_shader = GL_FRAGMENT_SHADER,
    geometry_shader = GL_GEOMETRY_SHADER,
    tessellation_evaluation_shader = GL_TESS_EVALUATION_SHADER,
    tessellation_control_shader = GL_TESS_CONTROL_SHADER,
    compute_shader = GL_COMPUTE_SHADER,
  };

  // OpenGL 4.5 Core Profile standard required constants

  // Minimum number of framebuffer color attachments
  constexpr i32 min_color_attachments = 8;
  constexpr i32 min_textures_per_stage = 16;

  [[nodiscard]] i32 get_max_texture_image_units();
  [[nodiscard]] i32 get_max_combined_texture_units();
  [[nodiscard]] i32 get_max_renderbuffer_size();
  [[nodiscard]] i32 get_max_color_attachments();
  [[nodiscard]] i32 get_max_draw_buffers();
  [[nodiscard]] i32 get_uniform_buffer_offset_alignment();
  [[nodiscard]] i32 get_min_map_buffer_alignment();

  void load();
  void load_functions();
  void load_constants();
  void install_debug_callback();

#if ANTON_DEBUG
  void _check_gl_errors();
  #define ANTON_CHECK_GL_ERRORS() ::anton_engine::opengl::_check_gl_errors()
#else
  #define ANTON_CHECK_GL_ERRORS() ((void)0)
#endif // ANTON_DEBUG

  // Notes:
  // - Base internal format uses unsigned normalized type. Size of the obtained texture is driver dependent.
} // namespace anton_engine::opengl

namespace anton_engine {
  template<>
  struct utils::enable_enum_add_operator<opengl::Attachment> {
    static constexpr bool value = true;
  };

  template<>
  struct utils::enable_enum_bitwise_or<opengl::Buffer_Mask> {
    static constexpr bool value = true;
  };

  template<>
  struct utils::enable_enum_bitwise_and<opengl::Buffer_Mask> {
    static constexpr bool value = true;
  };
} // namespace anton_engine

#include <rendering/opengl_enums_undefs.hpp>
