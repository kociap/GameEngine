#ifndef ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
#define ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE

#include "framebuffer_construct_info.hpp"
#include <cstdint>

namespace renderer {
    namespace framebuffer {
        class Framebuffer {
        public:
            Framebuffer(Framebuffer_construct_info const&);
            Framebuffer(Framebuffer&&);
            Framebuffer& operator=(Framebuffer&&);
            ~Framebuffer();

            Framebuffer() = delete;
            Framebuffer(Framebuffer const&) = delete;
            Framebuffer& operator=(Framebuffer const&) = delete;

            void bind();
            void unbind();
            void clear();
            void blit(Framebuffer&);
            uint32_t get_texture();

        private:
            Framebuffer_construct_info info;
            uint32_t framebuffer = 0;
            uint32_t depth_buffer = 0;
            uint32_t stencil_buffer = 0;
            uint32_t texture_color_buffer = 0;
        };
    } // namespace framebuffer
} // namespace renderer

#endif // !ENGINE_RENDERER_FRAMEBUFFER_HPP_INCLUDE
