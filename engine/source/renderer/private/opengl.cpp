#include "opengl.hpp"

#include "glad/glad.h"

#include "debug_macros.hpp"

namespace opengl {
    static int32_t max_combined_texture_units = 0;

    void load_opengl_parameters() {
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_units);
        CHECK_GL_ERRORS();
    }

    int32_t get_max_combined_texture_units() {
        return max_combined_texture_units;
    }

    void active_texture(uint32_t index) {
        glActiveTexture(GL_TEXTURE0 + index);
        CHECK_GL_ERRORS();
    }

    void bind_texture(uint32_t tex_enum, uint32_t handle) {
        glBindTexture(tex_enum, handle);
        CHECK_GL_ERRORS();
    }
} // namespace opengl