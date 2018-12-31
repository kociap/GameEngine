#ifndef ENGINE_RENDERED_FRAMEBUFFER_HPP_INCLUDE
#define ENGINE_RENDERED_FRAMEBUFFER_HPP_INCLUDE

#include <cstdint>

class Framebuffer {
public:
    Framebuffer(uint32_t width, uint32_t height);
    Framebuffer(Framebuffer&&);
    Framebuffer& operator=(Framebuffer&&);
    ~Framebuffer();

	Framebuffer() = delete;
	Framebuffer(Framebuffer const&) = delete;
    Framebuffer& operator=(Framebuffer const&) = delete;

    void bind();
    void unbind();
	void resize(uint32_t width, uint32_t height);
    uint32_t get_texture();

private:
    uint32_t framebuffer = 0;
    uint32_t depth_rbo = 0;
    uint32_t stencil_rbo = 0;
    uint32_t texture_color_buffer = 0;
};

#endif ENGINE_RENDERED_FRAMEBUFFER_HPP_INCLUDE