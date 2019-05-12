#include "opengl.hpp"

#include "glad/glad.h"

#include "debug_macros.hpp"
#include "utils/enum.hpp"

namespace opengl {
    static int32_t max_combined_texture_units = 0;
    static int32_t max_renderbuffer_size = 0;
    static int32_t max_color_attachments = 0;
    static int32_t max_draw_buffers = 0;

    void load_opengl_parameters() {
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_units);
        CHECK_GL_ERRORS();
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &max_renderbuffer_size);
        CHECK_GL_ERRORS();
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
        CHECK_GL_ERRORS();
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
        CHECK_GL_ERRORS();
    }

    uint32_t get_max_combined_texture_units() {
        return max_combined_texture_units;
    }

    uint32_t get_max_renderbuffer_size() {
        return max_renderbuffer_size;
    }

    uint32_t get_max_color_attachments() {
        return max_color_attachments;
    }

    uint32_t get_max_draw_buffers() {
        return max_draw_buffers;
    }

    void active_texture(uint32_t index) {
        glActiveTexture(GL_TEXTURE0 + index);
        CHECK_GL_ERRORS();
    }

    void bind_buffer(Buffer_Type buffer, uint32_t handle) {
        glBindBuffer(utils::enum_to_value(buffer), handle);
    }

    void bind_renderbuffer(uint32_t handle) {
        glBindRenderbuffer(GL_RENDERBUFFER, handle);
        CHECK_GL_ERRORS();
    }

    void bind_texture(uint32_t tex_enum, uint32_t handle) {
        glBindTexture(tex_enum, handle);
        CHECK_GL_ERRORS();
    }

    void bind_vertex_array(uint32_t handle) {
        glBindVertexArray(handle);
        CHECK_GL_ERRORS();
    }

    void blit_framebuffer(uint32_t s_x0, uint32_t s_y0, uint32_t s_x1, uint32_t s_y1, uint32_t d_x0, uint32_t d_y0, uint32_t d_x1, uint32_t d_y1,
                          Buffer_Mask mask, uint32_t filter) {
        glBlitFramebuffer(s_x0, s_y0, s_x1, s_y1, d_x0, d_y0, d_x1, d_y1, utils::enum_to_value(mask), filter);
        CHECK_GL_ERRORS();
    }

    void clear(Buffer_Mask buffers) {
        glClear(utils::enum_to_value(buffers));
        CHECK_GL_ERRORS();
    }

    void clear_color(Color c) {
        glClearColor(c.r, c.g, c.b, c.a);
        CHECK_GL_ERRORS();
    }

    void clear_color(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        CHECK_GL_ERRORS();
    }

    uint32_t create_shader(Shader_Type type) {
        // glCreateShader may generate GL_INVALID_ENUM, but since Shader_Type is an enum we
        //    will always have correct values (hopefully)
        return glCreateShader(utils::enum_to_value(type));
    }

    void delete_program(uint32_t program) {
        if (program != 0) {
            glDeleteProgram(program);
            CHECK_GL_ERRORS();
        }
    }

    void delete_shader(uint32_t shader) {
        if (shader != 0) {
            glDeleteShader(shader);
            CHECK_GL_ERRORS();
        }
    }

    void draw_elements(uint32_t mode, uint32_t count, uint64_t offset) {
        glDrawElements(mode, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset));
        CHECK_GL_ERRORS();
    }

    void draw_elements_instanced(uint32_t mode, uint32_t indices_count, uint32_t instances) {
        glDrawElementsInstanced(mode, indices_count, GL_UNSIGNED_INT, (void*)0, instances);
        CHECK_GL_ERRORS();
    }

    void enable_vertex_array_attribute(uint32_t index) {
        glEnableVertexAttribArray(index);
        CHECK_GL_ERRORS();
    }

    void framebuffer_renderbuffer(uint32_t target, Attachment attachment, uint32_t renderbuffer) {
        uint32_t gl_attachment = utils::enum_to_value(attachment);
        glFramebufferRenderbuffer(target, gl_attachment, GL_RENDERBUFFER, renderbuffer);
        CHECK_GL_ERRORS();
    }

    void framebuffer_texture_2D(uint32_t target, Attachment attachment, uint32_t tex_target, uint32_t texture, int32_t level) {
        uint32_t gl_attachment = utils::enum_to_value(attachment);
        glFramebufferTexture2D(target, gl_attachment, tex_target, texture, level);
        CHECK_GL_ERRORS();
    }

    void generate_mipmap(uint32_t target) {
        glGenerateMipmap(target);
        CHECK_GL_ERRORS();
    }

    void gen_buffers(uint32_t count, uint32_t* buffers) {
        glGenBuffers(count, buffers);
        CHECK_GL_ERRORS();
    }

    void gen_textures(uint32_t count, uint32_t* textures) {
        glGenTextures(count, textures);
        CHECK_GL_ERRORS();
    }

    void gen_renderbuffers(uint32_t count, uint32_t* renderbuffers) {
        glGenRenderbuffers(count, renderbuffers);
        CHECK_GL_ERRORS();
    }

    void gen_vertex_arrays(uint32_t count, uint32_t* vaos) {
        glGenVertexArrays(count, vaos);
        CHECK_GL_ERRORS();
    }

    uint32_t get_uniform_location(uint32_t program, char const* name) {
        uint32_t location = glGetUniformLocation(program, name);
        CHECK_GL_ERRORS();
        return location;
    }

    void renderbuffer_storage(uint32_t target, texture::Sized_Internal_Format internal_format, uint32_t width, uint32_t height) {
        glRenderbufferStorage(target, utils::enum_to_value(internal_format), width, height);
        CHECK_GL_ERRORS();
    }

    void renderbuffer_storage_multisample(uint32_t target, uint32_t samples, texture::Sized_Internal_Format internal_format, uint32_t width, uint32_t height) {
        glRenderbufferStorageMultisample(target, samples, utils::enum_to_value(internal_format), width, height);
        CHECK_GL_ERRORS();
    }

    void shader_source(uint32_t shader, uint64_t count, char const** strings, int32_t const* lengths) {
        glShaderSource(shader, count, strings, lengths);
        CHECK_GL_ERRORS();
    }

    void tex_image_2D(uint32_t target, int32_t level, texture::Base_Internal_Format, uint32_t width, uint32_t height, texture::Format pixels_format,
                      texture::Type pixels_type, void const* pixels);

    void tex_image_2D(uint32_t target, int32_t level, texture::Sized_Internal_Format sized_internal_format, uint32_t width, uint32_t height,
                      texture::Format pixels_format, texture::Type pixels_type, void const* pixels) {
        uint32_t internal_format = utils::enum_to_value(sized_internal_format);
        uint32_t format = utils::enum_to_value(pixels_format);
        uint32_t type = utils::enum_to_value(pixels_type);
        glTexImage2D(target, level, internal_format, width, height, 0, format, type, pixels);
        CHECK_GL_ERRORS();
    }

    void tex_image_2D_multisample(uint32_t target, uint32_t samples, texture::Sized_Internal_Format sized_internal_format, uint32_t width, uint32_t height,
                                  bool fixed_sample_locations /* = true */) {
        auto internal_format = utils::enum_to_value(sized_internal_format);
        glTexImage2DMultisample(target, samples, internal_format, width, height, fixed_sample_locations);
        CHECK_GL_ERRORS();
    }

    void vertex_array_attribute(uint32_t index, uint32_t size, uint32_t type, uint32_t stride, uint32_t offset, bool normalized) {
        // Double cast is a hack to suppress C4312 on MSVC. It's safe to cast 32bit int to void* in this context
        glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, reinterpret_cast<void*>(static_cast<uint64_t>(offset)));
        CHECK_GL_ERRORS();
    }

    void viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        glViewport(x, y, width, height);
        CHECK_GL_ERRORS();
    }

} // namespace opengl