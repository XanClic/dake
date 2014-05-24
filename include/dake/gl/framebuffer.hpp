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
        texture depth;
        GLenum *draw_buffers;


    public:
        framebuffer(int color_attachments);
        ~framebuffer(void);

        texture &operator[](int i);

        void resize(int w, int h);

        void bind(void);

        static void unbind(void) { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); glDrawBuffer(GL_BACK); }
};

}

}

#endif
