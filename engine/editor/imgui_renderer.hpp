#ifndef IMGUI_RENDERER_HPP_INCLUDE
#define IMGUI_RENDERER_HPP_INCLUDE

#include <shader.hpp>
#include <cstdint>

class Imgui_Renderer {
public:
    Imgui_Renderer();
    ~Imgui_Renderer();

    void new_frame();
    void render_ui();

private:
    Shader shader;
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;
};

#endif // !IMGUI_RENDERER_HPP_INCLUDE
