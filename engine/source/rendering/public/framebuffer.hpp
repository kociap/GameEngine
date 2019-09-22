#ifndef ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
#define ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE

#include <anton_stl/static_vector.hpp>
#include <array>
#include <cstdint>
#include <math/vector2.hpp>
#include <opengl.hpp>

namespace anton_engine {
    class Framebuffer {
    public:
        static constexpr uint64_t max_color_attachments = opengl::min_color_attachments;

        using Internal_Format = opengl::Sized_Internal_Format;

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

        struct Color_Buffer_Info {
            Internal_Format internal_format = Internal_Format::rgb8;
            Buffer_Type buffer_type = Buffer_Type::texture;
        };

        struct Depth_Buffer_Info {
            Internal_Format internal_format = Internal_Format::depth_component24;
            Buffer_Type buffer_type = Buffer_Type::renderbuffer;
            bool enabled = false;
        };

        struct Stencil_Buffer_Info {
            Internal_Format internal_format = Internal_Format::stencil_index8;
            Buffer_Type buffer_type = Buffer_Type::renderbuffer;
            bool enabled = false;
        };

        struct Construct_Info {
            anton_stl::Static_Vector<Color_Buffer_Info, max_color_attachments> color_buffers;
            Depth_Buffer_Info depth_buffer;
            Stencil_Buffer_Info stencil_buffer;
            int32_t width = 0;
            int32_t height = 0;
            int32_t samples = 0;
            bool multisampled = false;
        };

        static void bind(Framebuffer&, Bind_Mode = Bind_Mode::read_draw);
        static void bind_default(Bind_Mode = Bind_Mode::read_draw);
        static void blit(Framebuffer& from, Framebuffer& to, opengl::Buffer_Mask);

        Framebuffer(Construct_Info const&);
        Framebuffer(Framebuffer&&) noexcept;
        Framebuffer& operator=(Framebuffer&&) noexcept;
        ~Framebuffer();

        Framebuffer() = delete;
        Framebuffer(Framebuffer const&) = delete;
        Framebuffer& operator=(Framebuffer const&) = delete;

        void resize(int32_t width, int32_t height);
        Vector2 size() const;
        uint32_t get_color_texture(int32_t index) const;
        uint32_t get_depth_texture() const;

    private:
        void create_framebuffer();
        void delete_framebuffer();

        Construct_Info info;
        anton_stl::Static_Vector<uint32_t, max_color_attachments> color_buffers;
        int32_t active_color_buffers = 0;
        uint32_t framebuffer = 0;
        uint32_t depth_buffer = 0;
        uint32_t stencil_buffer = 0;
    };
} // namespace anton_engine

#endif // !ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
