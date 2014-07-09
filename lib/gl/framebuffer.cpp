#include <dake/gl/framebuffer.hpp>


dake::gl::framebuffer::framebuffer(int color_attachments):
    ca_count(color_attachments)
{
    draw_buffers = new GLenum[ca_count];
    for (int i = 0; i < ca_count; i++) {
        draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glGenFramebuffers(1, &id);
    bind();

    textures = new texture[ca_count];

    resize(1024, 1024);

    for (int i = 0; i < ca_count; i++) {
        textures[i].filter(GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i].get_glid(), 0);
    }

    depth.filter(GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.get_glid(), 0);
}


dake::gl::framebuffer::~framebuffer(void)
{
    glDeleteFramebuffers(1, &id);
    delete[] textures;
    delete[] draw_buffers;
}


void dake::gl::framebuffer::resize(int w, int h)
{
    for (int i = 0; i < ca_count; i++) {
        textures[i].format(GL_RGBA, w, h, GL_RGBA, GL_FLOAT);
    }
    depth.format(GL_DEPTH_COMPONENT24, w, h, GL_DEPTH_COMPONENT, GL_FLOAT);
}


void dake::gl::framebuffer::bind(void)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    glDrawBuffers(ca_count, draw_buffers);
}


dake::gl::texture &dake::gl::framebuffer::operator[](int i)
{
    return textures[i];
}


void dake::gl::framebuffer::mask(int i)
{
    draw_buffers[i] = GL_ZERO;
}


void dake::gl::framebuffer::unmask(int i)
{
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
}
