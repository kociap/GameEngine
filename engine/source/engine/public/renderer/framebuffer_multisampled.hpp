#ifndef ENGINE_RENDERER_FRAMEBUFFER_MULTISAMPLED_HPP_INCLUDE
#define ENGINE_RENDERER_FRAMEBUFFER_MULTISAMPLED_HPP_INCLUDE

#include "framebuffer.hpp"
#include <cstdint>

class Framebuffer_multisampled {
public:
    Framebuffer_multisampled(uint32_t width, uint32_t height, uint32_t samples);
    Framebuffer_multisampled(Framebuffer_multisampled&&);
    Framebuffer_multisampled& operator=(Framebuffer_multisampled&&);
    ~Framebuffer_multisampled();

    Framebuffer_multisampled() = delete;
    Framebuffer_multisampled(Framebuffer_multisampled const&) = delete;
    Framebuffer_multisampled& operator=(Framebuffer_multisampled const&) = delete;

    void bind();
    void unbind();
    void clear();
    void resize(uint32_t width, uint32_t height);
    void blit(Framebuffer&);

private:
    uint32_t framebuffer = 0;
    uint32_t depth_rbo = 0;
    uint32_t stencil_rbo = 0;
    uint32_t texture_color_buffer = 0;
    uint32_t samples = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};

#endif // !ENGINE_RENDERER_FRAMEBUFFER_MULTISAMPLED_HPP_INCLUDE