#include "framebuffer.hpp"

#include "debug_macros.hpp"
#include "glad/glad.h"
#include "opengl.hpp"
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// GL_INVALID_OPERATION is generated if type is one of GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5,
//     GL_UNSIGNED_SHORT_5_6_5_REV, or GL_UNSIGNED_INT_10F_11F_11F_REV, and format is not GL_RGB.
// GL_INVALID_OPERATION is generated if type is one of GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1,
//     GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV,
//     or GL_UNSIGNED_INT_5_9_9_9_REV, and format is neither GL_RGBA nor GL_BGRA.

// GL_INVALID_OPERATION is generated if target is not GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D, GL_TEXTURE_RECTANGLE, or GL_PROXY_TEXTURE_RECTANGLE,
//     and internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F.

// [DONE] GL_INVALID_OPERATION is generated if internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24,
//     or GL_DEPTH_COMPONENT32F, and format is not GL_DEPTH_COMPONENT.
// [DONE] GL_INVALID_OPERATION is generated if format is GL_DEPTH_COMPONENT and internalformat is not GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
//     GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F.

static opengl::texture::Format get_compatible_format(Framebuffer::Internal_Format internal_format) {
    using Format = Framebuffer::Internal_Format;
    if (internal_format == Format::depth_component16 || internal_format == Format::depth_component24 || internal_format == Format::depth_component32f ||
        internal_format == Format::depth_component32) {
        return opengl::texture::Format::depth_component;
    } else {
        return opengl::texture::Format::rgb;
    }
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

Framebuffer::Framebuffer(Construct_Info const& i): info(i) {
    if (info.width > opengl::get_max_renderbuffer_size() || info.height > opengl::get_max_renderbuffer_size()) {
        throw std::invalid_argument("Too big buffer size");
    }

    if (info.width == 0 || info.height == 0) {
        throw std::invalid_argument("One or both dimensions are 0");
    }

    GE_assert(!info.multisampled || info.samples != 0, "Multisampled framebuffer must have more than 0 samples");

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    active_color_buffers = info.color_buffers.size();
    color_buffers.resize(active_color_buffers);

    // Generate color buffer
    if (active_color_buffers > 0) {
        opengl::gen_textures(active_color_buffers, &color_buffers[0]);
        if (info.multisampled) {
            for (uint64_t i = 0; i < active_color_buffers; ++i) {
                uint32_t color_buffer_handle = color_buffers[i];
                auto internal_format = info.color_buffers[i].internal_format;
                opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, color_buffer_handle);
                opengl::tex_image_2D_multisample(GL_TEXTURE_2D_MULTISAMPLE, info.samples, internal_format, info.width, info.height);
                opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, color_buffer_handle, 0);
                CHECK_GL_ERRORS();
            }
        } else {
            for (uint32_t i = 0; i < info.color_buffers.size(); ++i) {
                uint32_t color_buffer_handle = color_buffers[i];
                auto internal_format = info.color_buffers[i].internal_format;
                auto compatible_format = get_compatible_format(internal_format);
                opengl::bind_texture(GL_TEXTURE_2D, color_buffer_handle);
                opengl::tex_image_2D(GL_TEXTURE_2D, 0, internal_format, info.width, info.height, compatible_format, opengl::texture::Type::signed_float,
                                     nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                CHECK_GL_ERRORS();
                opengl::bind_texture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color_buffer_handle, 0);
                CHECK_GL_ERRORS();
            }
        }

        containers::Static_Vector<uint32_t, max_color_attachments> active_color_attachments;
        for (uint32_t i = 0; i < color_buffers.size(); ++i) {
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
    opengl::Attachment depth_buffer_attachment = uses_depth_stencil ? opengl::Attachment::depth_stencil : opengl::Attachment::depth;
    if (depth_info.enabled) {
        if (depth_info.buffer_type == Buffer_Type::renderbuffer) {
            opengl::gen_renderbuffers(1, &depth_buffer);
            opengl::bind_renderbuffer(depth_buffer);
            if (info.multisampled) {
                opengl::renderbuffer_storage_multisample(GL_RENDERBUFFER, info.samples, depth_info.internal_format, info.width, info.height);
            } else {
                opengl::renderbuffer_storage(GL_RENDERBUFFER, depth_info.internal_format, info.width, info.height);
            }

            opengl::framebuffer_renderbuffer(GL_FRAMEBUFFER, depth_buffer_attachment, depth_buffer);
        } else {
            opengl::gen_textures(1, &depth_buffer);
            if (info.multisampled) {
                opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, depth_buffer);
                opengl::tex_image_2D_multisample(GL_TEXTURE_2D_MULTISAMPLE, info.samples, depth_info.internal_format, info.width, info.height);
                opengl::bind_texture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                opengl::framebuffer_texture_2D(GL_FRAMEBUFFER, depth_buffer_attachment, GL_TEXTURE_2D_MULTISAMPLE, depth_buffer, 0);
            } else {
                opengl::bind_texture(GL_TEXTURE_2D, depth_buffer);
                opengl::tex_image_2D(GL_TEXTURE_2D, 0, depth_info.internal_format, info.width, info.height, opengl::texture::Format::depth_component,
                                     opengl::texture::Type::signed_float, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                CHECK_GL_ERRORS();
                opengl::bind_texture(GL_TEXTURE_2D, 0);
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

    if (index >= active_color_buffers) {
        throw std::runtime_error("Color buffer with index " + std::to_string(index) + " is not bound");
    }

    return color_buffers[index];
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

void Framebuffer::blit(Framebuffer& from, Framebuffer& to, opengl::Buffer_Mask mask) {
    opengl::blit_framebuffer(0, 0, from.info.width, from.info.height, 0, 0, to.info.width, to.info.height, mask, GL_NEAREST);
}