#include <cassert>
#include <cstdint>

#include <dake/gl/elements_array.hpp>
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
    if (dake::gl::cur_va == this) {
        dake::gl::cur_va = nullptr;
    }

    glDeleteVertexArrays(1, &id);

    for (std::list<dake::gl::vertex_attrib *>::iterator i = attribs.begin(); i != attribs.end(); ++i) {
        delete *i;
    }

    delete index_buffer;
}


void dake::gl::vertex_array::set_elements(size_t count)
{
    n = count;
}


dake::gl::vertex_attrib *dake::gl::vertex_array::attrib(GLuint id)
{
    assert(id < (sizeof(uintmax_t) * 8));

    for (std::list<vertex_attrib *>::iterator i = attribs.begin(); i != attribs.end(); ++i) {
        if ((*i)->attrib == id) {
            return *i;
        }
    }

    bind();

    dake::gl::vertex_attrib *va = new dake::gl::vertex_attrib(this, id);
    attribs.push_back(va);

    glEnableVertexAttribArray(id);

    // FIXME, somehow
    dake::gl::cur_va = nullptr;

    return va;
}


void dake::gl::vertex_array::bind(void)
{
    if (dake::gl::cur_va == this) {
        return;
    }

    glBindVertexArray(id);
    dake::gl::cur_va = this;
}


void dake::gl::vertex_array::unbind_single(void)
{
}


void dake::gl::vertex_array::draw(GLenum type, int start_index)
{
    bind();

    if (index_buffer) {
        index_buffer->bind();
        glDrawElements(type, n, index_buffer->t, nullptr);
    } else {
        dake::gl::elements_array::unbind();
        glDrawArrays(type, start_index, n);
    }
}


dake::gl::elements_array *dake::gl::vertex_array::indices(void)
{
    if (!index_buffer) {
        index_buffer = new elements_array(this);
    }

    return index_buffer;
}
