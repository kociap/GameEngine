#include <rendering/opengl.hpp>

#include <rendering/glad.hpp>

#include <core/assert.hpp>
#include <core/atl/string.hpp>
#include <core/debug_macros.hpp> // CHECK_GL_ERRORS
#include <core/logging.hpp>
#include <stdexcept>
#include <core/utils/enum.hpp>

namespace anton_engine::opengl {
    static i32 max_combined_texture_units = 0;
    static i32 max_renderbuffer_size = 0;
    static i32 max_color_attachments = 0;
    static i32 max_draw_buffers = 0;
    static i32 uniform_buffer_offset_alignment = 0;
    static i32 max_fragment_shader_texture_units = 0;
    static i32 min_map_buffer_alignment = 0;

    void load() {
        load_functions();
        install_debug_callback();
        load_constants();
    }

    void load_functions() {
        if (!gladLoadGL()) {
            std::runtime_error("OpenGL not loaded");
        }
    }

    void load_constants() {
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_fragment_shader_texture_units);
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_units);
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &max_renderbuffer_size);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment);
        glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &min_map_buffer_alignment);
    }

    static void debug_callback(GLenum const source, GLenum const type, GLuint, GLenum const severity, GLsizei, GLchar const* const message, void const*) {
        auto stringify_source = [](GLenum const source) {
            switch (source) {
                case GL_DEBUG_SOURCE_API:
                    return u8"API";
                case GL_DEBUG_SOURCE_APPLICATION:
                    return u8"Application";
                case GL_DEBUG_SOURCE_SHADER_COMPILER:
                    return u8"Shader Compiler";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                    return u8"Window System";
                case GL_DEBUG_SOURCE_THIRD_PARTY:
                    return u8"Third Party";
                case GL_DEBUG_SOURCE_OTHER:
                    return u8"Other";
                default:
                    ANTON_UNREACHABLE();
            }
        };

        auto stringify_type = [](GLenum const type) {
            switch (type) {
                case GL_DEBUG_TYPE_ERROR:
                    return u8"Error";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    return u8"Deprecated Behavior";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                    return u8"Undefined Behavior";
                case GL_DEBUG_TYPE_PORTABILITY:
                    return u8"Portability";
                case GL_DEBUG_TYPE_PERFORMANCE:
                    return u8"Performance";
                case GL_DEBUG_TYPE_MARKER:
                    return u8"Marker";
                case GL_DEBUG_TYPE_PUSH_GROUP:
                    return u8"Push Group";
                case GL_DEBUG_TYPE_POP_GROUP:
                    return u8"Pop Group";
                case GL_DEBUG_TYPE_OTHER:
                    return u8"Other";
                default:
                    ANTON_UNREACHABLE();
            }
        };

        auto stringify_severity = [](GLenum const severity) {
            switch (severity) {
                case GL_DEBUG_SEVERITY_HIGH:
                    return u8"Fatal Error";
                case GL_DEBUG_SEVERITY_MEDIUM:
                    return u8"Error";
                case GL_DEBUG_SEVERITY_LOW:
                    return u8"Warning";
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    return u8"Note";
                default:
                    ANTON_UNREACHABLE();
            }
        };

        atl::String stringified_message =
            atl::String(stringify_severity(severity)) + " " + stringify_source(source) + " (" + stringify_type(type) + "): " + message;
        log_message(Log_Message_Severity::warning, stringified_message);
        if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
            DebugBreak();
        }
    }

    void install_debug_callback() {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_callback, nullptr);
        CHECK_GL_ERRORS();
    }

    i32 get_max_texture_image_units() {
        return max_fragment_shader_texture_units;
    }

    i32 get_max_combined_texture_units() {
        return max_combined_texture_units;
    }

    i32 get_max_renderbuffer_size() {
        return max_renderbuffer_size;
    }

    i32 get_max_color_attachments() {
        return max_color_attachments;
    }

    i32 get_max_draw_buffers() {
        return max_draw_buffers;
    }

    i32 get_uniform_buffer_offset_alignment() {
        return uniform_buffer_offset_alignment;
    }

    i32 get_min_map_buffer_alignment() {
        return min_map_buffer_alignment;
    }
} // namespace anton_engine::opengl
