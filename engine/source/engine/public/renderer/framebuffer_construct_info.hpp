#ifndef ENGINE_RENDERER_FRAMEBUFFER_CONSTRUCT_INFO_HPP_INCLUDE
#define ENGINE_RENDERER_FRAMEBUFFER_CONSTRUCT_INFO_HPP_INCLUDE

#include <cstdint>

namespace renderer {
    namespace framebuffer {
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

        struct Framebuffer_Construct_Info {
            uint32_t width = 0;
            uint32_t height = 0;
            uint32_t samples = 0;
            Attachment depth_buffer_attachment = Attachment::depth;
            Attachment stencil_buffer_attachment = Attachment::stencil;
            Buffer_Type depth_buffer_type = Buffer_Type::renderbuffer;
            Buffer_Type stencil_buffer_type = Buffer_Type::renderbuffer;
            bool color_buffer = true;
            bool depth_buffer = false;
            bool stencil_buffer = false;
            bool multisampled = false;
        };
    } // namespace framebuffer
} // namespace renderer

#endif // !ENGINE_RENDERER_FRAMEBUFFER_CONSTRUCT_INFO_HPP_INCLUDE