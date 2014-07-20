#ifndef DAKE__GL__ELEMENTS_ARRAY_HPP
#define DAKE__GL__ELEMENTS_ARRAY_HPP

#include <cstddef>
#include <cstdint>

#include "dake/gl/gl.hpp"


namespace dake
{

namespace gl
{

class vertex_array;
class elements_array;

extern elements_array *cur_ea;

class elements_array {
    private:
        GLuint buffer;
        vertex_array *va;
        int epv;
        size_t bpv;
        GLenum t;

        friend class vertex_array;


    public:
        elements_array(vertex_array *va);
        ~elements_array(void);

        void format(int elements_per_vertex, GLenum type = GL_UNSIGNED_INT);

        void data(void *ptr, size_t size = static_cast<size_t>(-1), GLenum usage = GL_STATIC_DRAW);

        void bind(void);
        static void unbind(void) { if (!cur_ea) return; glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); cur_ea = nullptr; }
};

}

}

#endif