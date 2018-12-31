#include "renderer/framebuffer.hpp"

#include "debug_macros.hpp"
#include "glad/glad.h"
#include <stdexcept>
#include <utility>

Framebuffer::Framebuffer(uint32_t width, uint32_t height) {
    if (width > GL_MAX_RENDERBUFFER_SIZE || height > GL_MAX_RENDERBUFFER_SIZE) {
        throw std::invalid_argument("Too big buffer size");
    }

	// Generate color buffer
    glGenTextures(1, &texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERRORS
	// Generate renderbuffers
    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    CHECK_GL_ERRORS
    //glGenRenderbuffers(1, &stencil_rbo);
    //CHECK_GL_ERRORS
    //glBindRenderbuffer(GL_RENDERBUFFER, stencil_rbo);
    //CHECK_GL_ERRORS
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    //CHECK_GL_ERRORS
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CHECK_GL_ERRORS
	// Generate framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_rbo);
    CHECK_GL_ERRORS

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
        CHECK_GL_ERRORS
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(Framebuffer&& fbo) {
    std::swap(framebuffer, fbo.framebuffer);
    std::swap(depth_rbo, fbo.depth_rbo);
    std::swap(stencil_rbo, fbo.stencil_rbo);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fbo) {
    std::swap(framebuffer, fbo.framebuffer);
    std::swap(depth_rbo, fbo.depth_rbo);
    std::swap(stencil_rbo, fbo.stencil_rbo);
    return *this;
}

Framebuffer::~Framebuffer() {
    if (depth_rbo != 0) {
        glDeleteRenderbuffers(1, &depth_rbo);
    }
    if (stencil_rbo != 0) {
        glDeleteRenderbuffers(1, &stencil_rbo);
    }
    if (texture_color_buffer != 0) {
        glDeleteTextures(1, &texture_color_buffer);
    }
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
    }
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t Framebuffer::get_texture() {
    return texture_color_buffer;
}