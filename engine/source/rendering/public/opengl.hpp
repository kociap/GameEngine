#ifndef RENDERER_OPENGL_HPP_INCLUDE
#define RENDERER_OPENGL_HPP_INCLUDE

#include <color.hpp>
#include <cstdint>
#include <opengl_enums_defs.hpp>
#include <utils/enum.hpp>

namespace anton_engine::opengl {
    // Uses unsigned normalized type.
    // Size of the obtained texture is driver dependent.
    enum class Base_Internal_Format : uint32_t {
        depth_component = GL_DEPTH_COMPONENT,
        depth_stencil = GL_DEPTH_STENCIL,
        stencil_index = GL_STENCIL_INDEX,
        red = GL_RED,
        rg = GL_RG,
        rgb = GL_RGB,
        rgba = GL_RGBA,
    };

    enum class Sized_Internal_Format : uint32_t {
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

    enum class Compressed_Internal_Format : uint32_t {
        // TODO compressed
    };

    enum class Format : uint32_t {
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

    enum class Type : uint32_t {
        unsigned_byte = GL_UNSIGNED_BYTE,
        signed_byte = GL_BYTE,
        unsigned_short = GL_UNSIGNED_SHORT,
        signed_short = GL_SHORT,
        unsigned_int = GL_UNSIGNED_INT,
        signed_int = GL_INT,
        half_float = GL_HALF_FLOAT,
        signed_float = GL_FLOAT,
    };

    enum class Attachment : uint32_t {
        color_attachment_0 = GL_COLOR_ATTACHMENT0,
        depth_attachment = GL_DEPTH_ATTACHMENT,
        depth_stencil_attachment = GL_DEPTH_STENCIL_ATTACHMENT,
        stencil_attachment = GL_STENCIL_ATTACHMENT,
    };

    enum class Buffer_Mask : uint32_t {
        color_buffer_bit = GL_COLOR_BUFFER_BIT,
        depth_buffer_bit = GL_DEPTH_BUFFER_BIT,
        stencil_buffer_bit = GL_STENCIL_BUFFER_BIT,
    };

    enum class Buffer_Type : uint32_t {
        array_buffer = GL_ARRAY_BUFFER,
        atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
        copy_read_buffer = GL_COPY_READ_BUFFER,
        copy_write_buffer = GL_COPY_WRITE_BUFFER,
        dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
        element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
        pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
        pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
        query_buffer = GL_QUERY_BUFFER,
        shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
        texture_buffer = GL_TEXTURE_BUFFER,
        transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
        uniform_buffer = GL_UNIFORM_BUFFER,
    };

    enum class Shader_Type : uint32_t {
        vertex_shader = GL_VERTEX_SHADER,
        fragment_shader = GL_FRAGMENT_SHADER,
        geometry_shader = GL_GEOMETRY_SHADER,
        tessellation_evaluation_shader = GL_TESS_EVALUATION_SHADER,
        tessellation_control_shader = GL_TESS_CONTROL_SHADER,
        compute_shader = GL_COMPUTE_SHADER,
    };

    enum class Texture_Type : uint32_t {
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

    enum class Texture_Filter : uint32_t {
        nearest = GL_NEAREST,
        linear = GL_LINEAR,
        nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
        nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
        linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
        linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR,
    };

    // OpenGL 4.5 Core Profile standard required constants

    // Minimum number of framebuffer color attachments
    constexpr uint32_t min_color_attachments = 8;

    [[nodiscard]] int32_t get_max_combined_texture_units();
    [[nodiscard]] int32_t get_max_renderbuffer_size();
    [[nodiscard]] int32_t get_max_color_attachments();
    [[nodiscard]] int32_t get_max_draw_buffers();

    void active_texture(uint32_t index);
    void bind_buffer(Buffer_Type, uint32_t handle);
    void bind_framebuffer(uint32_t target, uint32_t buffer);
    void bind_renderbuffer(uint32_t handle);
    void bind_texture(Texture_Type texture, uint32_t handle);
    void bind_vertex_array(uint32_t handle);
    void blit_framebuffer(int32_t src_x0, int32_t src_y0, int32_t src_x1, int32_t src_y1, int32_t dst_x0, int32_t dst_y0, int32_t dst_x1, int32_t dst_y1,
                          Buffer_Mask, uint32_t filter);
    // Size in bytes
    void buffer_data(Buffer_Type target, int64_t size, void* data, uint32_t usage);
    void clear(Buffer_Mask buffers);
    void clear_color(Color);
    void clear_color(float red, float green, float blue, float alpha);
    uint32_t create_shader(Shader_Type);
    void delete_program(uint32_t program);
    void delete_shader(uint32_t shader);
    void draw_arrays(uint32_t mode, int32_t first, int32_t count);
    // offset specifies the offset from the beginning of the enabled index array
    void draw_elements(uint32_t mode, int32_t count, int64_t offset = 0);
    // offset specifies the offset from the beginning of the enabled index array
    void draw_elements_instanced(uint32_t mode, int32_t indices_count, int64_t offset, int32_t instances);
    void enable_vertex_array_attribute(uint32_t index);
    void framebuffer_renderbuffer(uint32_t target, Attachment, uint32_t renderbuffer);
    void framebuffer_texture_2D(uint32_t target, Attachment, uint32_t tex_target, uint32_t texture, int32_t level);
    void generate_mipmap(uint32_t target);
    // n is the number of buffer objects to be generated. May not be negative.
    void gen_buffers(int32_t n, uint32_t* buffers);
    void gen_textures(int32_t count, uint32_t* textures);
    void gen_renderbuffers(int32_t count, uint32_t* renderbuffers);
    void gen_vertex_arrays(int32_t count, uint32_t* vertex_arrays);
    // -1 may mean a uniform removed by optimizations
    int32_t get_uniform_location(uint32_t program, char const* name);
    void renderbuffer_storage(uint32_t target, Sized_Internal_Format, int32_t width, int32_t height);
    void renderbuffer_storage_multisample(uint32_t target, int32_t samples, Sized_Internal_Format, int32_t width, int32_t height);
    void shader_source(uint32_t shader, int32_t count, char const** strings, int32_t const* lengths);
    void tex_image_2D(uint32_t target, int32_t level, Base_Internal_Format, int32_t width, int32_t height, Format pixels_format, Type pixels_type,
                      void const* pixels);
    void tex_image_2D(uint32_t target, int32_t level, Sized_Internal_Format, int32_t width, int32_t height, Format pixels_format, Type pixels_type,
                      void const* pixels);
    void tex_image_2D_multisample(uint32_t target, int32_t samples, Sized_Internal_Format, int32_t width, int32_t height, bool fixed_sample_locations = true);
    void vertex_array_attribute(uint32_t index, int32_t size, uint32_t type, int32_t stride, int64_t offset, bool normalized = false);
    // Lower left corner of the viewport, its width and height
    // Both width and height may not be negative
    void viewport(int32_t x, int32_t y, int32_t width, int32_t height);

    void load_functions();
    void load_constants();
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
