#include <framebuffer.hpp>

#include <anton_assert.hpp>
#include <anton_stl/utility.hpp>
#include <debug_macros.hpp> // CHECK_GL_ERRORS
#include <glad.hpp>
#include <opengl.hpp>

#include <stdexcept>
#include <string> // std::to_string

namespace anton_engine {
    void Framebuffer::bind(Framebuffer& fb, Bind_Mode const bm) {
        if (bm == Bind_Mode::read_draw) {
            glBindFramebuffer(GL_FRAMEBUFFER, fb.framebuffer);
        } else if (bm == Bind_Mode::read) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.framebuffer);
        } else {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.framebuffer);
        }
    }

    void Framebuffer::bind_default(Bind_Mode const bm) {
        if (bm == Bind_Mode::read_draw) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else if (bm == Bind_Mode::read) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        } else {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
    }

    void Framebuffer::blit(Framebuffer& from, Framebuffer& to, opengl::Buffer_Mask const mask) {
        opengl::blit_framebuffer(0, 0, from.info.width, from.info.height, 0, 0, to.info.width, to.info.height, mask, GL_NEAREST);
    }

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

            anton_stl::Static_Vector<u32, max_color_attachments> active_color_attachments;
            for (u32 i = 0; i < color_buffers.size(); ++i) {
                active_color_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
            }
            glDrawBuffers(active_color_buffers, &active_color_attachments[0]);
        } else {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        Depth_Buffer_Info depth_info = info.depth_buffer;
        bool uses_depth_stencil =
            depth_info.internal_format == Internal_Format::depth24_stencil8 || depth_info.internal_format == Internal_Format::depth32f_stencil8;
        opengl::Attachment depth_buffer_attachment = uses_depth_stencil ? opengl::Attachment::depth_stencil_attachment : opengl::Attachment::depth_attachment;
        if (depth_info.enabled) {
            if (depth_info.buffer_type == Buffer_Type::renderbuffer) {
                glGenRenderbuffers(1, &depth_buffer);
                glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
                if (info.multisampled) {
                    opengl::renderbuffer_storage_multisample(GL_RENDERBUFFER, info.samples, depth_info.internal_format, info.width, info.height);
                } else {
                    opengl::renderbuffer_storage(GL_RENDERBUFFER, depth_info.internal_format, info.width, info.height);
                }

                opengl::framebuffer_renderbuffer(GL_FRAMEBUFFER, depth_buffer_attachment, depth_buffer);
            } else {
                glGenTextures(1, &depth_buffer);
                if (info.multisampled) {
                    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &depth_buffer);
                    GLenum const internal_format = utils::enum_to_value(depth_info.internal_format);
                    glTextureStorage2DMultisample(depth_buffer, info.samples, internal_format, info.width, info.height, GL_TRUE);
                    opengl::framebuffer_texture_2D(GL_FRAMEBUFFER, depth_buffer_attachment, GL_TEXTURE_2D_MULTISAMPLE, depth_buffer, 0);
                } else {
                    GLenum const internal_format = utils::enum_to_value(depth_info.internal_format);
                    glTextureStorage2D(depth_buffer, 1, internal_format, info.width, info.height);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTextureParameteri(depth_buffer, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    opengl::framebuffer_texture_2D(GL_FRAMEBUFFER, depth_buffer_attachment, GL_TEXTURE_2D, depth_buffer, 0);
                }
            }
        }

        // TODO implement
        if (info.stencil_buffer.enabled) {
            //glGenRenderbuffers(1, &stencil_buffer);
            //CHECK_GL_ERRORS();
            //glBindRenderbuffer(GL_RENDERBUFFER, stencil_buffer);
            //CHECK_GL_ERRORS();
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
            //CHECK_GL_ERRORS();
            //glBindRenderbuffer(GL_RENDERBUFFER, 0);
            CHECK_GL_ERRORS();
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        if (info.multisampled) {
            throw std::runtime_error("Framebuffer is multisampled. Unable to get texture");
        }

        if (index >= active_color_buffers || index < 0) {
            throw std::runtime_error("Color buffer with index " + std::to_string(index) + " is not bound");
        }

        return color_buffers[index];
    }

    u32 Framebuffer::get_depth_texture() const {
        if (info.multisampled) {
            throw std::runtime_error("Framebuffer is multisampled. Unable to get texture");
        }

        if (!info.depth_buffer.enabled) {
            throw std::runtime_error("No depth buffer bound");
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
        switch (bind_mode) {
            case Framebuffer::Bind_Mode::read_draw:
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.get_framebuffer_gl_handle());
                break;
            case Framebuffer::Bind_Mode::read:
                glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.get_framebuffer_gl_handle());
                break;
            case Framebuffer::Bind_Mode::draw:
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.get_framebuffer_gl_handle());
                break;
        }
    }

    void bind_framebuffer(Framebuffer const* framebuffer, Framebuffer::Bind_Mode const bind_mode) {
        switch (bind_mode) {
            case Framebuffer::Bind_Mode::read_draw:
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->get_framebuffer_gl_handle());
                break;
            case Framebuffer::Bind_Mode::read:
                glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->get_framebuffer_gl_handle());
                break;
            case Framebuffer::Bind_Mode::draw:
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->get_framebuffer_gl_handle());
                break;
        }
    }

    void blit_framebuffer(Framebuffer& dest, Framebuffer const& source, opengl::Buffer_Mask const mask) {
        Framebuffer::Construct_Info const source_info = source.get_construct_info();
        Framebuffer::Construct_Info const dest_info = dest.get_construct_info();
        opengl::blit_framebuffer(0, 0, source_info.width, source_info.height, 0, 0, dest_info.width, dest_info.height, mask, GL_NEAREST);
    }

    void blit_framebuffer(Framebuffer* dest, Framebuffer const* source, opengl::Buffer_Mask const mask) {
        Framebuffer::Construct_Info const source_info = source->get_construct_info();
        Framebuffer::Construct_Info const dest_info = dest->get_construct_info();
        opengl::blit_framebuffer(0, 0, source_info.width, source_info.height, 0, 0, dest_info.width, dest_info.height, mask, GL_NEAREST);
    }
} // namespace anton_engine
