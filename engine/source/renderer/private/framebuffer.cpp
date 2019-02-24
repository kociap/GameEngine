#include "framebuffer.hpp"

#include "debug_macros.hpp"
#include "glad/glad.h"
#include <stdexcept>
#include <utility>

namespace renderer {
    void Framebuffer::bind(Framebuffer& fb, Bind_Mode bm) {
        if (bm == Bind_Mode::read_draw) {
            glBindFramebuffer(GL_FRAMEBUFFER, fb.framebuffer);
        } else if (bm == Bind_Mode::read) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.framebuffer);
        } else if (bm == Bind_Mode::draw) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.framebuffer);
        }
        CHECK_GL_ERRORS();
    }

    void Framebuffer::bind_default(Bind_Mode bm) {
        if (bm == Bind_Mode::read_draw) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else if (bm == Bind_Mode::read) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        } else if (bm == Bind_Mode::draw) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
        CHECK_GL_ERRORS();
    }

    Framebuffer::Framebuffer(Construct_Info const& i) : info(i) {
        if (info.width > GL_MAX_RENDERBUFFER_SIZE || info.height > GL_MAX_RENDERBUFFER_SIZE) {
            throw std::invalid_argument("Too big buffer size");
        }

        if (info.width == 0 || info.height == 0) {
            throw std ::invalid_argument("One or both dimensions are 0");
        }

        GE_assert(!info.multisampled || info.samples != 0, "Multisampled framebuffer must have more than 0 samples");

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // Generate color buffer
        if (info.color_buffer) {
            glGenTextures(1, &color_buffer);
            CHECK_GL_ERRORS();
            if (info.multisampled) {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_buffer);
                CHECK_GL_ERRORS();
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, info.samples, GL_RGB, info.width, info.height, GL_TRUE);
                CHECK_GL_ERRORS();
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                CHECK_GL_ERRORS();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, color_buffer, 0);
                CHECK_GL_ERRORS();
            } else {
                glBindTexture(GL_TEXTURE_2D, color_buffer);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.width, info.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                CHECK_GL_ERRORS();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);
                CHECK_GL_ERRORS();
            }
        } else {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        if (info.depth_buffer) {
            if (info.depth_buffer_type == Buffer_Type::renderbuffer) {
                // TODO change buffer type based on info instead of using depth_stencil
                glGenRenderbuffers(1, &depth_buffer);
                glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
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
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_buffer);
                    CHECK_GL_ERRORS();
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, info.samples, GL_DEPTH_COMPONENT, info.width, info.height, GL_TRUE);
                    CHECK_GL_ERRORS();
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                    CHECK_GL_ERRORS();
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_buffer, 0);
                    CHECK_GL_ERRORS();
                } else {
                    glBindTexture(GL_TEXTURE_2D, depth_buffer);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, info.width, info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    CHECK_GL_ERRORS();
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer, 0);
                    CHECK_GL_ERRORS();
                }
            }
        }

        // TODO implement
        if (info.stencil_buffer) {
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
            glDeleteRenderbuffers(1, &depth_buffer);
        }
        if (stencil_buffer != 0) {
            glDeleteRenderbuffers(1, &stencil_buffer);
        }
        if (color_buffer != 0) {
            glDeleteTextures(1, &color_buffer);
        }
        if (framebuffer != 0) {
            glDeleteFramebuffers(1, &framebuffer);
        }
    }

    uint32_t Framebuffer::get_color_texture() const {
        if (info.multisampled) {
            throw std::runtime_error("Framebuffer is multisampled. Unable to get texture");
        }

		if (!info.color_buffer) {
            throw std::runtime_error("No color buffer bound");
        }

        return color_buffer;
    }

	uint32_t Framebuffer::get_depth_texture() const {
        if (info.multisampled) {
            throw std::runtime_error("Framebuffer is multisampled. Unable to get texture");
		}

		if (!info.depth_buffer) {
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