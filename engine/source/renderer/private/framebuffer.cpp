#include "framebuffer.hpp"

#include "debug_macros.hpp"
#include "glad/glad.h"
#include "opengl.hpp"
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace renderer {
    static uint32_t internal_format_to_gl_constant(Framebuffer::Internal_Format format) {
        // clang-format off
        using Format = Framebuffer::Internal_Format;
        if (format == Format::rgb) { return GL_RGB; }
        if (format == Format::rgb16f) { return GL_RGB16F; }
		if (format == Format::rgb32f) { return GL_RGB32F; }
		if (format == Format::rgba) { return GL_RGBA; }
		throw std::invalid_argument("Invalid framebuffer's internal format"); // In case I forget to add ifs for new enum values
        // clang-format on
    }

	static uint32_t internal_format_to_gl_type(Framebuffer::Internal_Format format) {
		// clang-format off
        using Format = Framebuffer::Internal_Format;
        if (format == Format::rgb || format == Format::rgba) { return GL_UNSIGNED_BYTE; }
        if (format == Format::rgb16f || format == Format::rgb32f) { return GL_FLOAT; }
		// clang-format on
	}

    void Framebuffer::bind(Framebuffer& fb, Bind_Mode bm) {
        if (bm == Bind_Mode::read_draw) {
            glBindFramebuffer(GL_FRAMEBUFFER, fb.framebuffer);
        } else if (bm == Bind_Mode::read) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.framebuffer);
        } else {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.framebuffer);
        }
        CHECK_GL_ERRORS();
    }

    void Framebuffer::bind_default(Bind_Mode bm) {
        if (bm == Bind_Mode::read_draw) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else if (bm == Bind_Mode::read) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        } else {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
        CHECK_GL_ERRORS();
    }

    Framebuffer::Framebuffer(Construct_Info const& i) : info(i) {
        if (info.width > opengl::get_max_renderbuffer_size() || info.height > opengl::get_max_renderbuffer_size()) {
            throw std::invalid_argument("Too big buffer size");
        }

        if (info.width == 0 || info.height == 0) {
            throw std::invalid_argument("One or both dimensions are 0");
        }

        GE_assert(!info.multisampled || info.samples != 0, "Multisampled framebuffer must have more than 0 samples");

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        for (uint32_t i = 0; i < info.color_buffers.size(); ++i) {
            if (info.color_buffers[i].enabled) {
                color_buffer_index_to_active_map[i] = active_color_buffers;
                ++active_color_buffers;
            } else {
                color_buffer_index_to_active_map[i] = color_buffers.size() + 1;
            }
        }

        // Generate color buffer
        if (active_color_buffers > 0) {
            glGenTextures(active_color_buffers, &color_buffers[0]);
            CHECK_GL_ERRORS();
            if (info.multisampled) {
                for (uint32_t i = 0; i < info.color_buffers.size(); ++i) {
                    if (info.color_buffers[i].enabled) {
                        uint32_t active = color_buffer_index_to_active_map[i];
                        uint32_t color_buffer_handle = color_buffers[active];
                        uint32_t internal_format = internal_format_to_gl_constant(info.color_buffers[i].internal_format);
                        opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, color_buffer_handle);
                        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, info.samples, internal_format, info.width, info.height, GL_TRUE);
                        CHECK_GL_ERRORS();
                        opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + active, GL_TEXTURE_2D_MULTISAMPLE, color_buffer_handle, 0);
                        CHECK_GL_ERRORS();
                    }
                }
            } else {
                for (uint32_t i = 0; i < info.color_buffers.size(); ++i) {
                    if (info.color_buffers[i].enabled) {
                        uint32_t active = color_buffer_index_to_active_map[i];
                        uint32_t color_buffer_handle = color_buffers[active];
                        uint32_t internal_format = internal_format_to_gl_constant(info.color_buffers[i].internal_format);
                        uint32_t format = internal_format_to_gl_type(info.color_buffers[i].internal_format);
                        opengl::bind_texture(GL_TEXTURE_2D, color_buffer_handle);
                        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, info.width, info.height, 0, internal_format == GL_RGBA ? GL_RGBA : GL_RGB, format, nullptr);
                        CHECK_GL_ERRORS();
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        CHECK_GL_ERRORS();
                        opengl::bind_texture(GL_TEXTURE_2D, 0);
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + active, GL_TEXTURE_2D, color_buffer_handle, 0);
                        CHECK_GL_ERRORS();
                    }
                }
            }

            std::vector<uint32_t> active_color_attachments;
            active_color_attachments.reserve(active_color_buffers);
            for (uint32_t i = 0; i < color_buffers.size(); ++i) {
                uint32_t attachment_index = color_buffer_index_to_active_map[i];
                if (attachment_index < color_buffers.size()) {
                    active_color_attachments.push_back(GL_COLOR_ATTACHMENT0 + attachment_index);
                }
            }
            glDrawBuffers(active_color_buffers, &active_color_attachments[0]);

        } else {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        if (info.depth_buffer.enabled) {
            if (info.depth_buffer.buffer_type == Buffer_Type::renderbuffer) {
                // TODO change buffer type based on info instead of using depth_stencil
                glGenRenderbuffers(1, &depth_buffer);
                opengl::bind_renderbuffer(depth_buffer);
                if (info.multisampled) {
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, info.samples, GL_DEPTH24_STENCIL8, info.width, info.height);
                } else {
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.width, info.height);
                }
                CHECK_GL_ERRORS();
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
                CHECK_GL_ERRORS();
            } else {
                glGenTextures(1, &depth_buffer);
                if (info.multisampled) {
                    opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, depth_buffer);
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, info.samples, GL_DEPTH_COMPONENT, info.width, info.height, GL_TRUE);
                    CHECK_GL_ERRORS();
                    opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_buffer, 0);
                    CHECK_GL_ERRORS();
                } else {
                    opengl::bind_texture(GL_TEXTURE_2D, depth_buffer);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, info.width, info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    CHECK_GL_ERRORS();
                    opengl::bind_texture(GL_TEXTURE_2D, 0);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer, 0);
                    CHECK_GL_ERRORS();
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

        // Check framebuffer errors
        GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (framebuffer_status == GL_FRAMEBUFFER_UNDEFINED) {
            throw std::runtime_error("Framebuffer undefined");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
            throw std::runtime_error("Framebuffer incomplete attachment");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
            throw std::runtime_error("Framebuffer incomplete, missing attachment");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
            throw std::runtime_error("Framebuffer incomplete, missing draw buffer");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
            throw std::runtime_error("Framebuffer incomplete, read buffer without color attachment");
        } else if (framebuffer_status == GL_FRAMEBUFFER_UNSUPPORTED) {
            throw std::runtime_error(
                "Framebuffer unsupported: combination of internal formats of the attached images violates an implementation-dependent set of restrictions");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
            throw std::runtime_error("Framebuffer incomplete multisample");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) {
            throw std::runtime_error("Framebuffer incomplete layer targets");
        } else if (framebuffer_status == 0) {
            CHECK_GL_ERRORS();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Framebuffer::Framebuffer(Framebuffer&& fbo) noexcept {
        info = fbo.info;
        std::swap(framebuffer, fbo.framebuffer);
        std::swap(depth_buffer, fbo.depth_buffer);
        std::swap(stencil_buffer, fbo.stencil_buffer);
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& fbo) noexcept {
        info = fbo.info;
        std::swap(framebuffer, fbo.framebuffer);
        std::swap(depth_buffer, fbo.depth_buffer);
        std::swap(stencil_buffer, fbo.stencil_buffer);
        return *this;
    }

    Framebuffer::~Framebuffer() {
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

    uint32_t Framebuffer::get_color_texture(uint32_t index) const {
        if (info.multisampled) {
            throw std::runtime_error("Framebuffer is multisampled. Unable to get texture");
        }

        uint32_t mapped_index = color_buffer_index_to_active_map[index];

        if (mapped_index > color_buffers.size()) {
            throw std::runtime_error("Color buffer with index " + std::to_string(index) + " is not bound");
        }

        return color_buffers[mapped_index];
    }

    uint32_t Framebuffer::get_depth_texture() const {
        if (info.multisampled) {
            throw std::runtime_error("Framebuffer is multisampled. Unable to get texture");
        }

        if (!info.depth_buffer.enabled) {
            throw std::runtime_error("No depth buffer bound");
        }

        return depth_buffer;
    }

    void Framebuffer::blit(Framebuffer& fb, Buffer_Mask bm) {
        uint32_t gl_buffer_mask = (bm & Buffer_Mask::color ? GL_COLOR_BUFFER_BIT : 0) | (bm & Buffer_Mask::depth ? GL_DEPTH_BUFFER_BIT : 0) |
                                  (bm & Buffer_Mask::stencil ? GL_STENCIL_BUFFER_BIT : 0);
        glBlitFramebuffer(0, 0, info.width, info.height, 0, 0, fb.info.width, fb.info.height, gl_buffer_mask, GL_NEAREST);
        CHECK_GL_ERRORS();
    }
} // namespace renderer