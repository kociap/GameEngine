#include <opengl.hpp>

#include <glad.hpp>

#include <debug_macros.hpp>
#include <utils/enum.hpp>

namespace opengl {
    static int32_t max_combined_texture_units = 0;
    static int32_t max_renderbuffer_size = 0;
    static int32_t max_color_attachments = 0;
    static int32_t max_draw_buffers = 0;

    void load_functions() {
        if (!gladLoadGL()) {
            std::runtime_error("OpenGL not loaded");
        }
    }

    void load_constants() {
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_units);
        CHECK_GL_ERRORS();
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &max_renderbuffer_size);
        CHECK_GL_ERRORS();
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
        CHECK_GL_ERRORS();
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
        CHECK_GL_ERRORS();
    }

    int32_t get_max_combined_texture_units() {
        return max_combined_texture_units;
    }

    int32_t get_max_renderbuffer_size() {
        return max_renderbuffer_size;
    }

    int32_t get_max_color_attachments() {
        return max_color_attachments;
    }

    int32_t get_max_draw_buffers() {
        return max_draw_buffers;
    }

    void active_texture(uint32_t index) {
        glActiveTexture(GL_TEXTURE0 + index);
        CHECK_GL_ERRORS();
    }

    void bind_buffer(Buffer_Type buffer, uint32_t handle) {
        glBindBuffer(utils::enum_to_value(buffer), handle);
        CHECK_GL_ERRORS();
    }
    
    void bind_framebuffer(uint32_t target, uint32_t framebuffer) {
        glBindFramebuffer(target, framebuffer);
        CHECK_GL_ERRORS();
    }

    void bind_renderbuffer(uint32_t handle) {
        glBindRenderbuffer(GL_RENDERBUFFER, handle);
        CHECK_GL_ERRORS();
    }

    void bind_texture(Texture_Type texture, uint32_t handle) {
        glBindTexture(utils::enum_to_value(texture), handle);
        CHECK_GL_ERRORS();
    }

    void bind_vertex_array(uint32_t handle) {
        glBindVertexArray(handle);
        CHECK_GL_ERRORS();
    }

    void blit_framebuffer(int32_t s_x0, int32_t s_y0, int32_t s_x1, int32_t s_y1, int32_t d_x0, int32_t d_y0, int32_t d_x1, int32_t d_y1,
                          Buffer_Mask mask, uint32_t filter) {
        glBlitFramebuffer(s_x0, s_y0, s_x1, s_y1, d_x0, d_y0, d_x1, d_y1, utils::enum_to_value(mask), filter);
        CHECK_GL_ERRORS();
    }

    void buffer_data(Buffer_Type target, int64_t size, void* data, uint32_t usage) {
        GE_assert(size >= 0, "Size of the buffer's data store may not be negative.");
        glBufferData(utils::enum_to_value(target), size, data, usage);
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

    void draw_arrays(uint32_t mode, int32_t first, int32_t count) {
        GE_assert(count >= 0, "The number of indices may not be negative.");
        glDrawArrays(mode, first, count);
        CHECK_GL_ERRORS();
    }

    void draw_elements(uint32_t mode, int32_t count, int64_t offset) {
        glDrawElements(mode, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset));
        CHECK_GL_ERRORS();
    }

    void draw_elements_instanced(uint32_t mode, int32_t indices_count, int64_t offset, int32_t instances) {
        GE_assert(indices_count >= 0, "The number of indices may not be negative.");
        GE_assert(instances >= 0, "The number of instances may not be negative.");
        glDrawElementsInstanced(mode, indices_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset), instances);
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

    void gen_buffers(int32_t n, uint32_t* buffers) {
        GE_assert(n >= 0, "The number of buffer objects to be generated may not be negative.");
        glGenBuffers(n, buffers);
        CHECK_GL_ERRORS();
    }

    void gen_textures(int32_t count, uint32_t* textures) {
        glGenTextures(count, textures);
        CHECK_GL_ERRORS();
    }

    void gen_renderbuffers(int32_t n, uint32_t* renderbuffers) {
        GE_assert(n >= 0, "The number of renderbuffer objects to be generated may not be negative.");
        glGenRenderbuffers(n, renderbuffers);
        CHECK_GL_ERRORS();
    }

    void gen_vertex_arrays(int32_t n, uint32_t* vaos) {
        GE_assert(n >= 0, "The number of vertex array objects to be generated may not be negative.");
        glGenVertexArrays(n, vaos);
        CHECK_GL_ERRORS();
    }

    int32_t get_uniform_location(uint32_t program, char const* name) {
        int32_t location = glGetUniformLocation(program, name);
        CHECK_GL_ERRORS();
        return location;
    }

    void renderbuffer_storage(uint32_t target, Sized_Internal_Format internal_format, int32_t width, int32_t height) {
        GE_assert(width >= 0 && height >= 0, "Renderbuffer's storage width and height may not be less than 0.");
        glRenderbufferStorage(target, utils::enum_to_value(internal_format), width, height);
        CHECK_GL_ERRORS();
    }

    void renderbuffer_storage_multisample(uint32_t target, int32_t samples, Sized_Internal_Format internal_format, int32_t width, int32_t height) {
        GE_assert(width >= 0 && height >= 0, "width and height may not be less than 0.");
        GE_assert(samples >= 0, "Multisampled renderbuffer's samples may not be less than 0.");
        glRenderbufferStorageMultisample(target, samples, utils::enum_to_value(internal_format), width, height);
        CHECK_GL_ERRORS();
    }

    void shader_source(uint32_t shader, int32_t count, char const** strings, int32_t const* lengths) {
        GE_assert(count >= 0, "The number of shader sources may not be negative.");
        glShaderSource(shader, count, strings, lengths);
        CHECK_GL_ERRORS();
    }

    void tex_image_2D(uint32_t target, int32_t level, Base_Internal_Format, int32_t width, int32_t height, Format pixels_format, Type pixels_type,
                      void const* pixels);

    void tex_image_2D(uint32_t target, int32_t level, Sized_Internal_Format sized_internal_format, int32_t width, int32_t height, Format pixels_format,
                      Type pixels_type, void const* pixels) {
        GE_assert(width >= 0 && height >= 0, "width and height supplied to opengl::tex_image_2D may not be less than 0.");
        GE_assert(level >= 0, "mipmap level may not be less than 0.");
        auto internal_format = utils::enum_to_value(sized_internal_format);
        auto format = utils::enum_to_value(pixels_format);
        auto type = utils::enum_to_value(pixels_type);
        // glTexImage2D is inconsistent. Takes format as GLint instead of GLenum.
        glTexImage2D(target, level, static_cast<GLint>(internal_format), width, height, 0, format, type, pixels);
        CHECK_GL_ERRORS();
    }

    void tex_image_2D_multisample(uint32_t target, int32_t samples, Sized_Internal_Format sized_internal_format, int32_t width, int32_t height,
                                  bool fixed_sample_locations /* = true */) {
        GE_assert(samples >= 0 && width >= 0 && height >= 0, "samples, width and height supplied to opengl::tex_image_2D_multisample may not be less than 0");
        auto internal_format = utils::enum_to_value(sized_internal_format);
        glTexImage2DMultisample(target, samples, internal_format, width, height, fixed_sample_locations);
        CHECK_GL_ERRORS();
    }

    void vertex_array_attribute(uint32_t index, int32_t size, uint32_t type, int32_t stride, int64_t offset, bool normalized) {
        GE_assert(stride >= 0, "Stride may not be negative.");
        glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, reinterpret_cast<void*>(offset));
        CHECK_GL_ERRORS();
    }

    void viewport(int32_t x, int32_t y, int32_t width, int32_t height) {
        GE_assert(width >= 0 && height >= 0, "width or height supplied to opengl::viewport is negative");
        glViewport(x, y, width, height);
        CHECK_GL_ERRORS();
    }

} // namespace opengl
