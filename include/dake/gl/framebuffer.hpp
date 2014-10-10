#ifndef DAKE__GL__FRAMEBUFFER_H
#define DAKE__GL__FRAMEBUFFER_H

#include "dake/gl/gl.hpp"
#include "dake/gl/texture.hpp"


namespace dake
{

namespace gl
{

class framebuffer;

extern framebuffer *current_fb;


class framebuffer {
    public:
        enum depth_stencil_mode {
            // No depth or stencil buffer
            NO_DEPTH_OR_STENCIL,
            // Only depth buffer
            DEPTH_ONLY,
            // Only stencil buffer
            STENCIL_ONLY,
            // Only stencil or depth buffer are used at a time
            // (allows combined stencil/depth buffer)
            STENCIL_XOR_DEPTH,
            // Both stencil and depth buffer may be used at a time
            // (not supported (yet))
            STENCIL_AND_DEPTH,
        };


    private:
        int ca_count;
        GLuint id;
        texture *textures;
        texture *depth_buffer, *stencil_buffer;
        GLenum *draw_buffers;
        GLenum *formats;
        int width, height;
        depth_stencil_mode dsm;
        bool depth_buffer_in_stencil_mode = false;
        int spp = 1;


    public:
        framebuffer(int color_attachments, GLenum format = GL_RGBA, depth_stencil_mode depth_stencil = DEPTH_ONLY, int multisample = 0);
        ~framebuffer(void);

        texture &operator[](int i);
        texture &depth(void);
        texture &stencil(void);

        void resize(int w, int h);
        void color_format(int i, GLenum format);
        void depth_format(GLenum format);

        void bind(void);
        void blit(int sx = 0, int sy = 0, int sw = -1, int sh = -1, int dx = 0, int dy = 0, int dw = -1, int dh = -1, GLenum mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);

        static framebuffer *current(void) { return current_fb; }

        void mask(int i);
        void unmask(int i);

        static void unbind(void) { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); glDrawBuffer(GL_BACK); current_fb = nullptr; }
};

}

}

#endif
