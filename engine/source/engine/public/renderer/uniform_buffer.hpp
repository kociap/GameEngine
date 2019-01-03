#ifndef ENGINE_RENDERER_UNIFORM_BUFFER_HPP_INCLUDE
#define ENGINE_RENDERER_UNIFORM_BUFFER_HPP_INCLUDE

#include "renderer/buffers.hpp"
#include <cstdint>

class Uniform_buffer {
public:
    Uniform_buffer();
    Uniform_buffer(Uniform_buffer const&);
    Uniform_buffer& operator=(Uniform_buffer const&);
    Uniform_buffer(Uniform_buffer&&);
    Uniform_buffer& operator=(Uniform_buffer&&);
    ~Uniform_buffer();

	Uniform_buffer(uint32_t size, Buffer_draw_type);

private:
    uint32_t uniform_buffer = 0;
};

#endif // !ENGINE_RENDERER_UNIFORM_BUFFER_HPP_INCLUDE