#include "imgui_renderer.hpp"

#include "debug_macros.hpp"
#include "engine.hpp"
#include "framebuffer.hpp"
#include "glad/glad.h"
#include "imgui.h"
#include "math/transform.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "shader_file.hpp"

static uint32_t texture_from_font(ImGuiIO& io) {
    // TODO this code according to ImGui is shit, fix it
    unsigned char* pixels;
    int width, height;
    // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    uint32_t font_tex;
    glGenTextures(1, &font_tex);
    glBindTexture(GL_TEXTURE_2D, font_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECK_GL_ERRORS();

    io.Fonts->TexID = (ImTextureID)(intptr_t)font_tex;
    return font_tex;
}

Shader create_default_shader() {
    const GLchar* vertex_shader_glsl_450_core =
        "#version 450 core\n"
        "layout (location = 0) in vec2 Position;\n"
        "layout (location = 1) in vec2 UV;\n"
        "layout (location = 2) in vec4 Color;\n"
        "uniform mat4 projection_matrix;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "    Frag_UV = UV;\n"
        "    Frag_Color = Color;\n"
        "    gl_Position = projection_matrix * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader_glsl_450_core =
        "#version 450 core\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "uniform sampler2D tex;\n"
        "layout (location = 0) out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "    Out_Color = Frag_Color * texture(tex, Frag_UV.st);\n"
        "}\n";

    Shader_File vertex_shader("ImGui Vertex Shader", opengl::Shader_Type::vertex_shader, vertex_shader_glsl_450_core);
    Shader_File fragment_shader("ImGui Fragment Shader", opengl::Shader_Type::fragment_shader, fragment_shader_glsl_450_core);

    Shader shader = create_shader(fragment_shader, vertex_shader);
    shader.use();
    shader.set_int("tex", 0);

    return shader;
}

Imgui_Renderer::Imgui_Renderer(): shader(create_default_shader()) {
    namespace imgui = ImGui;
    imgui::CreateContext();

    opengl::gen_buffers(1, &vbo);
    opengl::gen_buffers(1, &ebo);
    opengl::gen_vertex_arrays(1, &vao);
    opengl::bind_vertex_array(vao);
    opengl::bind_buffer(opengl::Buffer_Type::array_buffer, vbo);
    opengl::bind_buffer(opengl::Buffer_Type::element_array_buffer, ebo);
    opengl::vertex_array_attribute(0, 2, GL_FLOAT, sizeof(ImDrawVert), 0);
    opengl::enable_vertex_array_attribute(0);
    opengl::vertex_array_attribute(1, 2, GL_FLOAT, sizeof(ImDrawVert), offsetof(ImDrawVert, uv));
    opengl::enable_vertex_array_attribute(1);
    opengl::vertex_array_attribute(2, 4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), offsetof(ImDrawVert, col), true);
    opengl::enable_vertex_array_attribute(2);

    // Create default font
    // TODO this code is not pinnacle of programming
    ImGuiIO& io = imgui::GetIO();
    io.Fonts->AddFontDefault();
    texture_from_font(io);

    io.ConfigWindowsResizeFromEdges = true;

    // Global window style settings
    ImGuiStyle& style = imgui::GetStyle();
    style.WindowRounding = 0.0f;
}

Imgui_Renderer::~Imgui_Renderer() {
    ImGui::DestroyContext();
}

void Imgui_Renderer::new_frame() {
    ImGui::NewFrame();
}

static void setup_renderer_state() {
    CHECK_GL_ERRORS();
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    CHECK_GL_ERRORS();
}

void Imgui_Renderer::render_ui() {
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    // Save state
    GLint last_polygon_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
    GLint last_viewport[4];
    glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLenum last_blend_src_rgb;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    GLenum last_blend_dst_rgb;
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    GLenum last_blend_src_alpha;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    GLenum last_blend_dst_alpha;
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    GLenum last_blend_equation_rgb;
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    GLenum last_blend_equation_alpha;
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    Framebuffer::bind_default();
    opengl::viewport(0, 0, draw_data->DisplaySize.x, draw_data->DisplaySize.y);
    opengl::clear_color(0.0f, 0.0f, 0.0f, 1.0f);
    opengl::clear(opengl::Buffer_Mask::color_buffer_bit);

    setup_renderer_state();
    opengl::bind_vertex_array(vao);
    opengl::bind_buffer(opengl::Buffer_Type::array_buffer, vbo);
    opengl::bind_buffer(opengl::Buffer_Type::element_array_buffer, ebo);

    float left = draw_data->DisplayPos.x;
    float right = left + draw_data->DisplaySize.x;
    float top = draw_data->DisplayPos.y;
    float bottom = top + draw_data->DisplaySize.y;
    Matrix4 projection = math::transform::orthographic(left, right, bottom, top, -1.0f, 1.0f);
    shader.use();
    shader.set_matrix4("projection_matrix", projection);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        size_t idx_buffer_offset = 0;

        // Upload vertex/index buffers
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data,
                     GL_STREAM_DRAW);
        CHECK_GL_ERRORS();

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL) {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    setup_renderer_state();
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            } else {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f) {
                    // Apply scissor/clipping rectangle
                    // Works if glClipControl is not set to GL_UPPER_LEFT
                    glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

                    opengl::active_texture(0);
                    opengl::bind_texture(opengl::Texture_Type::texture_2D, reinterpret_cast<uint32_t>(pcmd->TextureId));
                    opengl::draw_elements(GL_TRIANGLES, pcmd->ElemCount, idx_buffer_offset);
                }
            }
            idx_buffer_offset += pcmd->ElemCount * sizeof(ImDrawIdx);
        }
    }

    // Restore state
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (last_enable_cull_face)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    if (last_enable_depth_test)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}