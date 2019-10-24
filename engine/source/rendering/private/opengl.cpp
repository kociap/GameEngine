#include <opengl.hpp>

#include <glad.hpp>

#include <anton_assert.hpp>
#include <anton_stl/string.hpp>
#include <debug_macros.hpp> // CHECK_GL_ERRORS
#include <logging.hpp>
#include <stdexcept>
#include <utils/enum.hpp>

namespace anton_engine::opengl {
    static i32 max_combined_texture_units = 0;
    static i32 max_renderbuffer_size = 0;
    static i32 max_color_attachments = 0;
    static i32 max_draw_buffers = 0;
    static i32 uniform_buffer_offset_alignment = 0;
    static i32 max_fragment_shader_texture_units = 0;

    void load() {
        load_functions();
        load_constants();
        install_debug_callback();
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

        anton_stl::String stringified_message =
            anton_stl::String(stringify_severity(severity)) + " " + stringify_source(source) + " (" + stringify_type(type) + "): " + message;
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

    u32 create_shader(Shader_Type type) {
        // glCreateShader may generate GL_INVALID_ENUM, but since Shader_Type is an enum we
        //    will always have correct values (hopefully)
        return glCreateShader(utils::enum_to_value(type));
    }

    void framebuffer_renderbuffer(u32 target, Attachment attachment, u32 renderbuffer) {
        u32 gl_attachment = utils::enum_to_value(attachment);
        glFramebufferRenderbuffer(target, gl_attachment, GL_RENDERBUFFER, renderbuffer);
        CHECK_GL_ERRORS();
    }

    void framebuffer_texture_2D(u32 target, Attachment attachment, u32 tex_target, u32 texture, i32 level) {
        u32 gl_attachment = utils::enum_to_value(attachment);
        glFramebufferTexture2D(target, gl_attachment, tex_target, texture, level);
        CHECK_GL_ERRORS();
    }

    void renderbuffer_storage(u32 target, Sized_Internal_Format internal_format, i32 width, i32 height) {
        ANTON_ASSERT(width >= 0 && height >= 0, "Renderbuffer's storage width and height may not be less than 0.");
        glRenderbufferStorage(target, utils::enum_to_value(internal_format), width, height);
        CHECK_GL_ERRORS();
    }

    void renderbuffer_storage_multisample(u32 target, i32 samples, Sized_Internal_Format internal_format, i32 width, i32 height) {
        ANTON_ASSERT(width >= 0 && height >= 0, "width and height may not be less than 0.");
        ANTON_ASSERT(samples >= 0, "Multisampled renderbuffer's samples may not be less than 0.");
        glRenderbufferStorageMultisample(target, samples, utils::enum_to_value(internal_format), width, height);
        CHECK_GL_ERRORS();
    }

    void shader_source(u32 shader, i32 count, char const** strings, i32 const* lengths) {
        ANTON_ASSERT(count >= 0, "The number of shader sources may not be negative.");
        glShaderSource(shader, count, strings, lengths);
        CHECK_GL_ERRORS();
    }
} // namespace anton_engine::opengl
