#include <framebuffer.hpp>

#include <anton_assert.hpp>
#include <anton_stl/utility.hpp>
#include <debug_macros.hpp> // CHECK_GL_ERRORS
#include <glad.hpp>
#include <opengl.hpp>

#include <stdexcept>
#include <string> // std::to_string

namespace anton_engine {
    void Framebuffer::create_framebuffer() {
        if (info.width > opengl::get_max_renderbuffer_size() || info.height > opengl::get_max_renderbuffer_size()) {
            throw std::invalid_argument("Too big buffer size");
        }

        if (info.width <= 0 || info.height <= 0) {
            throw std::invalid_argument("One or both dimensions are less than or equal 0");
        }

        ANTON_ASSERT(!info.multisampled || info.samples > 0, "Multisampled framebuffer must have more than 0 samples");

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        active_color_buffers = static_cast<i32>(info.color_buffers.size());
        color_buffers.resize(active_color_buffers);

        // TODO add support for renderbuffer color attachments

        // Generate color buffer
        if (active_color_buffers > 0) {
            if (info.multisampled) {
                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, active_color_buffers, color_buffers.data());
                for (i32 i = 0; i < active_color_buffers; ++i) {
                    GLenum const internal_format = utils::enum_to_value(info.color_buffers[i].internal_format);
                    glTextureStorage2DMultisample(color_buffers[i], info.samples, internal_format, info.width, info.height, GL_TRUE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i), GL_TEXTURE_2D_MULTISAMPLE, color_buffers[i], 0);
                }
            } else {
                glCreateTextures(GL_TEXTURE_2D, active_color_buffers, color_buffers.data());
                for (u32 i = 0; i < info.color_buffers.size(); ++i) {
                    GLenum const internal_format = utils::enum_to_value(info.color_buffers[i].internal_format);
                    glTextureStorage2D(color_buffers[i], 1, internal_format, info.width, info.height);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color_buffers[i], 0);
                }
            }

            u32 active_color_attachments[max_color_attachments];
            for (u32 i = 0; i < color_buffers.size(); ++i) {
                active_color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            glDrawBuffers(active_color_buffers, active_color_attachments);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
        } else {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        Depth_Buffer_Info depth_info = info.depth_buffer;
        if (depth_info.enabled) {
            bool const uses_depth_stencil =
                depth_info.internal_format == Internal_Format::depth24_stencil8 || depth_info.internal_format == Internal_Format::depth32f_stencil8;
            GLenum const depth_buffer_attachment = uses_depth_stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
            if (depth_info.buffer_type == Buffer_Type::renderbuffer) {
                glGenRenderbuffers(1, &depth_buffer);
                glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
                GLenum const internal_format = utils::enum_to_value(depth_info.internal_format);
                if (info.multisampled) {
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, info.samples, internal_format, info.width, info.height);
                } else {
                    glRenderbufferStorage(GL_RENDERBUFFER, internal_format, info.width, info.height);
                }

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, depth_buffer_attachment, GL_RENDERBUFFER, depth_buffer);
            } else {
                GLenum const internal_format = utils::enum_to_value(depth_info.internal_format);
                if (info.multisampled) {
                    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &depth_buffer);
                    glTextureStorage2DMultisample(depth_buffer, info.samples, internal_format, info.width, info.height, GL_TRUE);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, depth_buffer_attachment, GL_TEXTURE_2D_MULTISAMPLE, depth_buffer, 0);
                } else {
                    glCreateTextures(GL_TEXTURE_2D, 1, &depth_buffer);
                    glTextureStorage2D(depth_buffer, 1, internal_format, info.width, info.height);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, depth_buffer_attachment, GL_TEXTURE_2D, depth_buffer, 0);
                }
            }
        }

        // TODO implement
        if (info.stencil_buffer.enabled) {
            //glGenRenderbuffers(1, &stencil_buffer);
            //glBindRenderbuffer(GL_RENDERBUFFER, stencil_buffer);
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
            //glBindRenderbuffer(GL_RENDERBUFFER, 0);
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_buffer);
        }

        GLenum const le_fremebuffere_statouse = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (le_fremebuffere_statouse) {
            case GL_FRAMEBUFFER_UNDEFINED:
                throw std::runtime_error("Framebuffer undefined");
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                throw std::runtime_error("Framebuffer incomplete attachment");
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                throw std::runtime_error("Framebuffer incomplete, missing attachment");
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                throw std::runtime_error("Framebuffer incomplete, missing draw buffer");
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                throw std::runtime_error("Framebuffer incomplete, read buffer without color attachment");
            case GL_FRAMEBUFFER_UNSUPPORTED:
                throw std::runtime_error(
                    "Framebuffer unsupported: combination of internal formats of the attached images violates an implementation-dependent set of restrictions");
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                throw std::runtime_error("Framebuffer incomplete multisample");
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                throw std::runtime_error("Framebuffer incomplete layer targets");
            case 0:
                CHECK_GL_ERRORS();
        }
    }

    void Framebuffer::delete_framebuffer() {
        if (depth_buffer != 0) {
            if (info.depth_buffer.buffer_type == Buffer_Type::renderbuffer) {
                glDeleteRenderbuffers(1, &depth_buffer);
            } else {
                glDeleteTextures(1, &depth_buffer);
            }
        }
        if (stencil_buffer != 0) {
            glDeleteRenderbuffers(1, &stencil_buffer);
        }
        if (active_color_buffers > 0) {
            glDeleteTextures(active_color_buffers, &color_buffers[0]);
        }
        if (framebuffer != 0) {
            glDeleteFramebuffers(1, &framebuffer);
        }
    }

    Framebuffer::Framebuffer(Construct_Info const& i): info(i) {
        create_framebuffer();
    }

    Framebuffer::Framebuffer(Framebuffer&& fbo) noexcept {
        info = fbo.info;
        anton_stl::swap(framebuffer, fbo.framebuffer);
        anton_stl::swap(depth_buffer, fbo.depth_buffer);
        anton_stl::swap(stencil_buffer, fbo.stencil_buffer);
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& fbo) noexcept {
        info = fbo.info;
        anton_stl::swap(framebuffer, fbo.framebuffer);
        anton_stl::swap(depth_buffer, fbo.depth_buffer);
        anton_stl::swap(stencil_buffer, fbo.stencil_buffer);
        return *this;
    }

    Framebuffer::~Framebuffer() {
        delete_framebuffer();
    }

    void Framebuffer::resize(i32 const width, i32 const height) {
        delete_framebuffer();
        info.width = width;
        info.height = height;
        create_framebuffer();
    }

    Vector2 Framebuffer::size() const {
        return {static_cast<float>(info.width), static_cast<float>(info.height)};
    }

    u32 Framebuffer::get_color_texture(i32 const index) const {
        if (index >= active_color_buffers || index < 0) {
            throw std::runtime_error("Color buffer with index " + std::to_string(index) + " is not bound.");
        }

        return color_buffers[index];
    }

    anton_stl::Slice<u32 const> Framebuffer::get_color_textures() const {
        return color_buffers;
    }

    u32 Framebuffer::get_depth_texture() const {
        if (info.depth_buffer.buffer_type == Buffer_Type::renderbuffer) {
            throw std::runtime_error("Framebuffer uses renderbuffer as depth buffer. Unable to get texture.");
        }

        if (!info.depth_buffer.enabled) {
            throw std::runtime_error("No depth buffer bound.");
        }

        return depth_buffer;
    }

    u32 Framebuffer::get_framebuffer_gl_handle() const {
        return framebuffer;
    }

    Framebuffer::Construct_Info Framebuffer::get_construct_info() const {
        return info;
    }

    void bind_framebuffer(Framebuffer const& framebuffer, Framebuffer::Bind_Mode const bind_mode) {
        glBindFramebuffer(utils::enum_to_value(bind_mode), framebuffer.get_framebuffer_gl_handle());
    }

    void bind_framebuffer(Framebuffer const* framebuffer, Framebuffer::Bind_Mode const bind_mode) {
        glBindFramebuffer(utils::enum_to_value(bind_mode), framebuffer->get_framebuffer_gl_handle());
    }

    void blit_framebuffer(Framebuffer& dest, Framebuffer const& source, opengl::Buffer_Mask const mask) {
        Framebuffer::Construct_Info const source_info = source.get_construct_info();
        Framebuffer::Construct_Info const dest_info = dest.get_construct_info();
        glBlitFramebuffer(0, 0, source_info.width, source_info.height, 0, 0, dest_info.width, dest_info.height, utils::enum_to_value(mask), GL_NEAREST);
    }

    void blit_framebuffer(Framebuffer* dest, Framebuffer const* source, opengl::Buffer_Mask const mask) {
        Framebuffer::Construct_Info const source_info = source->get_construct_info();
        Framebuffer::Construct_Info const dest_info = dest->get_construct_info();
        glBlitFramebuffer(0, 0, source_info.width, source_info.height, 0, 0, dest_info.width, dest_info.height, utils::enum_to_value(mask), GL_NEAREST);
    }
} // namespace anton_engine
