#ifndef DAKE__GL__VERTEX_ATTRIB_HPP
#define DAKE__GL__VERTEX_ATTRIB_HPP

#include <cstddef>
#include <QtOpenGL>


namespace dake
{

namespace gl
{

class vertex_array;

class vertex_attrib
{
    private:
        GLuint buffer, attrib;
        bool buffer_reused = false;
        int epv;
        size_t bpv;
        GLenum t;
        vertex_array *va;

        friend class vertex_array;


    public:
        vertex_attrib(vertex_array *va, GLuint id);
        ~vertex_attrib(void);

        void reuse_buffer(vertex_attrib *va);
        void format(int elements_per_vertex, GLenum type = GL_FLOAT);

        void load(size_t stride = 0, off_t offset = 0);

        void data(void *ptr, size_t size = (size_t)-1, GLenum usage = GL_STATIC_DRAW);
};

}

}

#endif
