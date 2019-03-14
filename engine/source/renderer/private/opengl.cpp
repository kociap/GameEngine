#include "opengl.hpp"

#include "glad/glad.h"

#include "debug_macros.hpp"

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

    void vertex_array_attribute(uint32_t index, uint32_t size, uint32_t type, uint32_t stride, uint32_t offset, bool normalized) {
        glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, (void*)offset);
        CHECK_GL_ERRORS();
    }

    void enable_vertex_array_attribute(uint32_t index) {
        glEnableVertexAttribArray(index);
        CHECK_GL_ERRORS();
    }

    void draw_elements(uint32_t mode, uint32_t count) {
        glDrawElements(mode, count, GL_UNSIGNED_INT, (void*)0);
        CHECK_GL_ERRORS();
    }

    void draw_elements_instanced(uint32_t mode, uint32_t indices_count, uint32_t instances) {
        glDrawElementsInstanced(mode, indices_count, GL_UNSIGNED_INT, (void*)0, instances);
        CHECK_GL_ERRORS();
    }
} // namespace opengl