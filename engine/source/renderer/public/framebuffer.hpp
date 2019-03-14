#ifndef ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
#define ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE

#include "opengl.hpp"
#include <array>
#include <cstdint>

namespace renderer {
    class Framebuffer {
    public:
        enum class Draw_Mode {
            static_draw,
            streamed_draw,
            dynamic_draw,
        };

        enum class Bind_Mode {
            read,
            draw,
            read_draw,
        };

        enum class Buffer_Type {
            renderbuffer,
            texture,
        };

        enum class Attachment {
            color,
            depth_stencil,
            depth,
            stencil,
        };

        enum class Internal_Format {
            rgb,
            rgb16f,
            rgb32f,
			rgba,
        };

        enum Buffer_Mask : uint32_t {
            color = 1 << 0,
            depth = 1 << 1,
            stencil = 1 << 2,
        };

        struct Color_Buffer_Info {
            Internal_Format internal_format = Internal_Format::rgb;
            Buffer_Type buffer_type = Buffer_Type::texture;
            bool enabled = false;
        };

        struct Depth_Buffer_Info {
            Attachment attachment = Attachment::depth;
            Buffer_Type buffer_type = Buffer_Type::renderbuffer;
            bool enabled = false;
        };

        struct Stencil_Buffer_Info {
            Attachment attachment = Attachment::stencil;
            Buffer_Type buffer_type = Buffer_Type::renderbuffer;
            bool enabled = false;
        };

        struct Construct_Info {
            std::array<Color_Buffer_Info, opengl::min_color_attachments> color_buffers;
            Depth_Buffer_Info depth_buffer;
            Stencil_Buffer_Info stencil_buffer;
            uint32_t width = 0;
            uint32_t height = 0;
            uint32_t samples = 0;
            bool multisampled = false;
        };

        static void bind(Framebuffer&, Bind_Mode = Bind_Mode::read_draw);
        static void bind_default(Bind_Mode = Bind_Mode::read_draw);

        Framebuffer(Construct_Info const&);
        Framebuffer(Framebuffer&&) noexcept;
        Framebuffer& operator=(Framebuffer&&) noexcept;
        ~Framebuffer();

        Framebuffer() = delete;
        Framebuffer(Framebuffer const&) = delete;
        Framebuffer& operator=(Framebuffer const&) = delete;

        void clear();
        void blit(Framebuffer&, Buffer_Mask);
        uint32_t get_color_texture(uint32_t index) const;
        uint32_t get_depth_texture() const;

    private:
        Construct_Info info;
        std::array<uint32_t, opengl::min_color_attachments> color_buffers;
        std::array<uint32_t, opengl::min_color_attachments> color_buffer_index_to_active_map;
        uint32_t active_color_buffers = 0;
        uint32_t framebuffer = 0;
        uint32_t depth_buffer = 0;
        uint32_t stencil_buffer = 0;
    };
} // namespace renderer

#endif // !ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
