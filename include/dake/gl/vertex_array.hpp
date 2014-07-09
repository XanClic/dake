#ifndef DAKE__GL__VERTEX_ARRAY_HPP
#define DAKE__GL__VERTEX_ARRAY_HPP

#include <list>

#include "dake/gl/gl.hpp"


namespace dake
{

namespace gl
{

class vertex_attrib;
class vertex_array;
class elements_array;

extern vertex_array *cur_va;

class vertex_array
{
    private:
        GLuint id;
        size_t n;
        std::list<vertex_attrib *> attribs;
        elements_array *index_buffer = nullptr;

        friend class vertex_attrib;
        friend class elements_array;


        void unbind_single(void);


    public:
        vertex_array(void);
        ~vertex_array(void);

        void set_elements(size_t n);

        vertex_attrib *attrib(GLuint id);
        elements_array *indices(void);

        void bind(void);
        static void unbind(void) { if (!cur_va) return; cur_va->unbind_single(); glBindVertexArray(0); cur_va = nullptr; }

        void draw(GLenum type, int start_index = 0);
};

}

}

#endif
