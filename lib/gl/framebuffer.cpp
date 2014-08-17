#include <dake/gl/framebuffer.hpp>


namespace dake
{

namespace gl
{

dake::gl::framebuffer *current_fb = nullptr;

}

}


dake::gl::framebuffer::framebuffer(int color_attachments, GLenum format):
    ca_count(color_attachments)
{
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

    resize(1024, 1024);

    for (int i = 0; i < ca_count; i++) {
        textures[i].filter(GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i].get_glid(), 0);
    }

    depth_buffer.filter(GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer.get_glid(), 0);
}


dake::gl::framebuffer::~framebuffer(void)
{
    glDeleteFramebuffers(1, &id);
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
    depth_buffer.format(GL_DEPTH_COMPONENT24, w, h, GL_DEPTH_COMPONENT, GL_FLOAT);
}


void dake::gl::framebuffer::color_format(int i, GLenum format)
{
    formats[i] = format;

    resize(width, height);
}


void dake::gl::framebuffer::bind(void)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    glDrawBuffers(ca_count, draw_buffers);

    current_fb = this;
}


dake::gl::texture &dake::gl::framebuffer::operator[](int i)
{
    return textures[i];
}


dake::gl::texture &dake::gl::framebuffer::depth(void)
{
    return depth_buffer;
}


void dake::gl::framebuffer::mask(int i)
{
    draw_buffers[i] = GL_ZERO;
}


void dake::gl::framebuffer::unmask(int i)
{
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
}
