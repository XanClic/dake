#ifndef DAKE__GL__FRAMEBUFFER_H
#define DAKE__GL__FRAMEBUFFER_H

#include "dake/gl/gl.hpp"
#include "dake/gl/texture.hpp"


namespace dake
{

namespace gl
{

class framebuffer
{
    private:
        int ca_count;
        GLuint id;
        texture *textures;
        texture depth_buffer;
        GLenum *draw_buffers;
        GLenum *formats;
        int width, height;


    public:
        framebuffer(int color_attachments, GLenum format = GL_RGBA);
        ~framebuffer(void);

        texture &operator[](int i);
        texture &depth(void);

        void resize(int w, int h);
        void color_format(int i, GLenum format);

        void bind(void);

        void mask(int i);
        void unmask(int i);

        static void unbind(void) { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); glDrawBuffer(GL_BACK); }
};

}

}

#endif
