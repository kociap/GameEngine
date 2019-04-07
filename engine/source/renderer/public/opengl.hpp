#ifndef RENDERER_OPENGL_HPP_INCLUDE
#define RENDERER_OPENGL_HPP_INCLUDE

#include "opengl_enums_defs.hpp"
#include "utils/enum.hpp"
#include <cstdint>

namespace opengl {
    namespace texture {
        // Uses unsigned normalized type.
        // Size of the obtained texture is driver dependent
        enum class Base_Internal_Format {
            depth_component = GL_DEPTH_COMPONENT,
            depth_stencil = GL_DEPTH_STENCIL,
            stencil_index = GL_STENCIL_INDEX,
            red = GL_RED,
            rg = GL_RG,
            rgb = GL_RGB,
            rgba = GL_RGBA,
        };

        enum class Sized_Internal_Format {
            // unsigned normalized
            r8 = GL_R8,
            r16 = GL_R16,
            rg8 = GL_RG8,
            rg16 = GL_RG16,
            rgb4 = GL_RGB4,
            rgb8 = GL_RGB8,
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

        enum class Compressed_Internal_Format {
            // TODO compressed
        };

        enum class Format {
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

        enum class Type {
            unsigned_byte = GL_UNSIGNED_BYTE,
            signed_byte = GL_BYTE,
            unsigned_short = GL_UNSIGNED_SHORT,
            signed_short = GL_SHORT,
            unsigned_int = GL_UNSIGNED_INT,
            signed_int = GL_INT,
            half_float = GL_HALF_FLOAT,
            signed_float = GL_FLOAT,
        };
    } // namespace texture

    enum class Attachment {
        color0 = GL_COLOR_ATTACHMENT0,
        depth = GL_DEPTH_ATTACHMENT,
        depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT,
        stencil = GL_STENCIL_ATTACHMENT,
    };

    // OpenGL 4.5 Core Profile standard required constants

    // Minimum number of framebuffer color attachments
    constexpr uint32_t min_color_attachments = 8;

    int32_t get_max_combined_texture_units();
    int32_t get_max_renderbuffer_size();
    int32_t get_max_color_attachments();
    int32_t get_max_draw_buffers();

    void active_texture(uint32_t index);
    void bind_renderbuffer(uint32_t handle);
    void bind_texture(uint32_t tex_enum, uint32_t handle);
    void bind_vertex_array(uint32_t handle);
    void vertex_array_attribute(uint32_t index, uint32_t count, uint32_t type, uint32_t stride, uint32_t offset, bool normalized = false);
    void enable_vertex_array_attribute(uint32_t index);
    void draw_elements(uint32_t mode, uint32_t count);
    void draw_elements_instanced(uint32_t mode, uint32_t indices_count, uint32_t instances);
    void framebuffer_renderbuffer(uint32_t target, Attachment, uint32_t renderbuffer);
    void framebuffer_texture_2D(uint32_t target, Attachment, uint32_t tex_target, uint32_t texture, int32_t level);
    void gen_textures(uint32_t count, uint32_t* textures);
    void gen_renderbuffers(uint32_t count, uint32_t* renderbuffers);
    void renderbuffer_storage(uint32_t target, texture::Sized_Internal_Format, uint32_t width, uint32_t height);
    void renderbuffer_storage_multisample(uint32_t target, uint32_t samples, texture::Sized_Internal_Format, uint32_t width, uint32_t height);
    void tex_image_2D(uint32_t target, int32_t level, texture::Base_Internal_Format, uint32_t width, uint32_t height, texture::Format pixels_format,
                      texture::Type pixels_type, void const* pixels);
    void tex_image_2D(uint32_t target, int32_t level, texture::Sized_Internal_Format, uint32_t width, uint32_t height, texture::Format pixels_format,
                      texture::Type pixels_type, void const* pixels);
    void tex_image_2D_multisample(uint32_t target, uint32_t samples, texture::Sized_Internal_Format, uint32_t width, uint32_t height,
                                  bool fixed_sample_locations = true);

    void load_opengl_parameters();
} // namespace opengl

template <>
struct utils::enable_enum_add_operator<opengl::Attachment> {
    static constexpr bool value = true;
};

#include "opengl_enums_undefs.hpp"
#endif // !RENDERER_OPENGL_HPP_INCLUDE
