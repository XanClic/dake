#include <cassert>
#include <cstdint>

#include <dake/gl/gl.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>


namespace dake
{

namespace gl
{

dake::gl::vertex_array *cur_va = nullptr;

}

}


dake::gl::vertex_array::vertex_array(void)
{
    glGenVertexArrays(1, &id);
}


dake::gl::vertex_array::~vertex_array(void)
{
    if (dake::gl::cur_va == this)
        dake::gl::cur_va = nullptr;

    glDeleteVertexArrays(1, &id);

    for (std::list<dake::gl::vertex_attrib *>::iterator i = attribs.begin(); i != attribs.end(); ++i)
        delete *i;
}


void dake::gl::vertex_array::set_elements(size_t count)
{
    n = count;
}


dake::gl::vertex_attrib *dake::gl::vertex_array::attrib(GLuint id)
{
    assert(id < (sizeof(uintmax_t) * 8));

    for (std::list<vertex_attrib *>::iterator i = attribs.begin(); i != attribs.end(); ++i)
        if ((*i)->attrib == id)
            return *i;

    bind();

    dake::gl::vertex_attrib *va = new dake::gl::vertex_attrib(this, id);
    attribs.push_back(va);

    // FIXME, somehow
    dake::gl::cur_va = nullptr;

    return va;
}


static uintmax_t enabled_vertex_arrays;


void dake::gl::vertex_array::bind(void)
{
    if (dake::gl::cur_va == this)
        return;

    uintmax_t enable_vertex_arrays = 0;

    for (std::list<dake::gl::vertex_attrib *>::iterator i = attribs.begin(); i != attribs.end(); ++i) {
        enable_vertex_arrays |= UINTMAX_C(1) << (*i)->attrib;
    }

    uintmax_t difference = enable_vertex_arrays ^ enabled_vertex_arrays;

    for (GLuint id = 0; id < sizeof(uintmax_t) * 8; id++) {
        if (difference & (UINTMAX_C(1) << id)) {
            if (enable_vertex_arrays & (UINTMAX_C(1) << id)) {
                glEnableVertexAttribArray(id);
            } else {
                glDisableVertexAttribArray(id);
            }
        }
    }

    enabled_vertex_arrays = enable_vertex_arrays;

    glBindVertexArray(id);
    dake::gl::cur_va = this;
}


void dake::gl::vertex_array::unbind_single(void)
{
    for (std::list<dake::gl::vertex_attrib *>::iterator i = attribs.begin(); i != attribs.end(); i++) {
        glDisableVertexAttribArray((*i)->attrib);
    }

    enabled_vertex_arrays = 0;
}


void dake::gl::vertex_array::draw(GLenum type, int start_index)
{
    bind();
    glDrawArrays(type, start_index, n);
}
