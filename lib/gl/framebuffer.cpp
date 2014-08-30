#include <stdexcept>

#include <dake/gl/framebuffer.hpp>


namespace dake
{

namespace gl
{

dake::gl::framebuffer *current_fb = nullptr;

}

}


dake::gl::framebuffer::framebuffer(int color_attachments, GLenum format, depth_stencil_mode ds_mode):
    ca_count(color_attachments),
    dsm(ds_mode)
{
    if ((dsm == STENCIL_XOR_DEPTH) && !glext.has_extension(STENCIL_TEXTURING)) {
        dsm = STENCIL_AND_DEPTH;
    }

    if (dsm == STENCIL_AND_DEPTH) {
        throw std::invalid_argument("Depth/stencil mode STENCIL_AND_DEPTH is not supported");
    }

    draw_buffers = new GLenum[ca_count];
    for (int i = 0; i < ca_count; i++) {
        draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    formats = new GLenum[ca_count];
    for (int i = 0; i < ca_count; i++) {
        formats[i] = format;
    }

    glGenFramebuffers(1, &id);
    bind();

    textures = new texture[ca_count];

    switch (dsm) {
        case DEPTH_ONLY:
        case STENCIL_XOR_DEPTH:
            depth_buffer = new texture;
            stencil_buffer = nullptr;
            break;

        case STENCIL_ONLY:
            depth_buffer = nullptr;
            stencil_buffer = new texture;
            break;

        case NO_DEPTH_OR_STENCIL:
            depth_buffer = nullptr;
            stencil_buffer = nullptr;
            break;

        default:
            throw std::invalid_argument("Invalid value given for depth/stencil mode");
    }

    resize(1024, 1024);

    for (int i = 0; i < ca_count; i++) {
        textures[i].filter(GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i].get_glid(), 0);
    }

    if (depth_buffer) {
        depth_buffer->filter(GL_NEAREST);
        if (dsm == STENCIL_XOR_DEPTH) {
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_buffer->get_glid(), 0);
        } else {
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer->get_glid(), 0);
        }
    } else if (stencil_buffer) {
        stencil_buffer->filter(GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_buffer->get_glid(), 0);
    }

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer incomplete");
    }
}


dake::gl::framebuffer::~framebuffer(void)
{
    glDeleteFramebuffers(1, &id);
    delete depth_buffer;
    delete stencil_buffer;
    delete[] textures;
    delete[] formats;
    delete[] draw_buffers;
}


void dake::gl::framebuffer::resize(int w, int h)
{
    width = w;
    height = h;

    for (int i = 0; i < ca_count; i++) {
        textures[i].format(formats[i], w, h, GL_RGBA, GL_FLOAT);
    }

    if (depth_buffer) {
        if (dsm == STENCIL_XOR_DEPTH) {
            depth_buffer->format(GL_DEPTH24_STENCIL8, w, h, GL_DEPTH_COMPONENT, GL_FLOAT);
        } else {
            depth_buffer->format(GL_DEPTH_COMPONENT24, w, h, GL_DEPTH_COMPONENT, GL_FLOAT);
        }
    } else if (stencil_buffer) {
        stencil_buffer->format(GL_STENCIL_INDEX8, w, h, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE);
    }
}


void dake::gl::framebuffer::color_format(int i, GLenum format)
{
    formats[i] = format;

    resize(width, height);
}


void dake::gl::framebuffer::bind(void)
{
    if (current_fb != this) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    }

    glDrawBuffers(ca_count, draw_buffers);

    current_fb = this;
}


dake::gl::texture &dake::gl::framebuffer::operator[](int i)
{
    return textures[i];
}


dake::gl::texture &dake::gl::framebuffer::depth(void)
{
    if (!depth_buffer) {
        throw std::runtime_error("No depth buffer available");
    }

    if ((dsm == STENCIL_XOR_DEPTH) && depth_buffer_in_stencil_mode) {
        depth_buffer->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
        depth_buffer_in_stencil_mode = false;
    }

    return *depth_buffer;
}


dake::gl::texture &dake::gl::framebuffer::stencil(void)
{
    if ((dsm == NO_DEPTH_OR_STENCIL) || (dsm == DEPTH_ONLY)) {
        throw std::runtime_error("No stencil buffer available");
    }

    if ((dsm == STENCIL_XOR_DEPTH) && !depth_buffer_in_stencil_mode) {
        depth_buffer->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);
        depth_buffer_in_stencil_mode = true;
    }

    if (dsm == STENCIL_XOR_DEPTH) {
        return *depth_buffer;
    } else {
        return *stencil_buffer;
    }
}


void dake::gl::framebuffer::mask(int i)
{
    draw_buffers[i] = GL_ZERO;
}


void dake::gl::framebuffer::unmask(int i)
{
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
}
