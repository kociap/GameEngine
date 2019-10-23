#ifndef ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
#define ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/static_vector.hpp>
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
            i32 width = 0;
            i32 height = 0;
            i32 samples = 0;
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

        void resize(i32 width, i32 height);
        Vector2 size() const;
        u32 get_color_texture(i32 index) const;
        u32 get_depth_texture() const;

        u32 get_framebuffer_gl_handle() const;
        Construct_Info get_construct_info() const;

    private:
        void create_framebuffer();
        void delete_framebuffer();

        Construct_Info info;
        anton_stl::Static_Vector<u32, max_color_attachments> color_buffers;
        i32 active_color_buffers = 0;
        u32 framebuffer = 0;
        u32 depth_buffer = 0;
        u32 stencil_buffer = 0;
    };

    void bind_framebuffer(Framebuffer const&, Framebuffer::Bind_Mode = Framebuffer::Bind_Mode::read_draw);
    void bind_framebuffer(Framebuffer const*, Framebuffer::Bind_Mode = Framebuffer::Bind_Mode::read_draw);
    void blit_framebuffer(Framebuffer& dest, Framebuffer const& source, opengl::Buffer_Mask);
    void blit_framebuffer(Framebuffer* dest, Framebuffer const* source, opengl::Buffer_Mask);

} // namespace anton_engine

#endif // !ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
