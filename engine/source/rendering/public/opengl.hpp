#ifndef RENDERER_OPENGL_HPP_INCLUDE
#define RENDERER_OPENGL_HPP_INCLUDE

#include <anton_int.hpp>
#include <color.hpp>
#include <opengl_enums_defs.hpp>
#include <utils/enum.hpp>

namespace anton_engine::opengl {
    // Uses unsigned normalized type.
    // Size of the obtained texture is driver dependent.
    enum class Base_Internal_Format : u32 {
        depth_component = GL_DEPTH_COMPONENT,
        depth_stencil = GL_DEPTH_STENCIL,
        stencil_index = GL_STENCIL_INDEX,
        red = GL_RED,
        rg = GL_RG,
        rgb = GL_RGB,
        rgba = GL_RGBA,
    };

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

    enum class Shader_Type : u32 {
        vertex_shader = GL_VERTEX_SHADER,
        fragment_shader = GL_FRAGMENT_SHADER,
        geometry_shader = GL_GEOMETRY_SHADER,
        tessellation_evaluation_shader = GL_TESS_EVALUATION_SHADER,
        tessellation_control_shader = GL_TESS_CONTROL_SHADER,
        compute_shader = GL_COMPUTE_SHADER,
    };

    enum class Texture_Type : u32 {
        texture_1D = GL_TEXTURE_1D,
        texture_2D = GL_TEXTURE_2D,
        texture_3D = GL_TEXTURE_3D,
        texture_1D_array = GL_TEXTURE_1D_ARRAY,
        texture_2D_array = GL_TEXTURE_2D_ARRAY,
        texture_rectangle = GL_TEXTURE_RECTANGLE,
        texture_cube_map = GL_TEXTURE_CUBE_MAP,
        texture_cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
        texture_buffer = GL_TEXTURE_BUFFER,
        texture_2D_multisample = GL_TEXTURE_2D_MULTISAMPLE,
        texture_2D_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    };

    enum class Texture_Filter : u32 {
        nearest = GL_NEAREST,
        linear = GL_LINEAR,
        nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
        nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
        linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
        linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR,
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

    void blit_framebuffer(i32 src_x0, i32 src_y0, i32 src_x1, i32 src_y1, i32 dst_x0, i32 dst_y0, i32 dst_x1, i32 dst_y1, Buffer_Mask, u32 filter);
    u32 create_shader(Shader_Type);
    void framebuffer_renderbuffer(u32 target, Attachment, u32 renderbuffer);
    void framebuffer_texture_2D(u32 target, Attachment, u32 tex_target, u32 texture, i32 level);
    void renderbuffer_storage(u32 target, Sized_Internal_Format, i32 width, i32 height);
    void renderbuffer_storage_multisample(u32 target, i32 samples, Sized_Internal_Format, i32 width, i32 height);
    void shader_source(u32 shader, i32 count, char const** strings, i32 const* lengths);

    void load();
    void load_functions();
    void load_constants();
    void install_debug_callback();
} // namespace anton_engine::opengl

namespace anton_engine {
    template <>
    struct utils::enable_enum_add_operator<opengl::Attachment> {
        static constexpr bool value = true;
    };

    template <>
    struct utils::enable_enum_bitwise_or<opengl::Buffer_Mask> {
        static constexpr bool value = true;
    };

    template <>
    struct utils::enable_enum_bitwise_and<opengl::Buffer_Mask> {
        static constexpr bool value = true;
    };
} // namespace anton_engine

#include <opengl_enums_undefs.hpp>
#endif // !RENDERER_OPENGL_HPP_INCLUDE
