#ifndef DAKE__GL__VERTEX_ARRAY_HPP
#define DAKE__GL__VERTEX_ARRAY_HPP

#include <list>
#include <QtOpenGL>


namespace dake
{

namespace gl
{

class vertex_attrib;
class vertex_array;

extern vertex_array *cur_va;

class vertex_array
{
    private:
        GLuint id;
        size_t n;
        std::list<vertex_attrib *> attribs;

        friend class vertex_attrib;


        void unbind_single(void);


    public:
        vertex_array(void);
        ~vertex_array(void);

        void set_elements(size_t n);

        vertex_attrib *attrib(GLuint id);

        void bind(void);
        static void unbind(void) { cur_va->unbind_single(); glBindVertexArray(0); }

        void draw(GLenum type, int start_index = 0);
};

}

}

#endif
