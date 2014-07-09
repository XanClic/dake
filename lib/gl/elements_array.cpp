#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <dake/gl/elements_array.hpp>
#include <dake/gl/gl.hpp>
#include <dake/gl/vertex_array.hpp>


namespace dake
{

namespace gl
{

dake::gl::elements_array *cur_ea = nullptr;

}

}


dake::gl::elements_array::elements_array(vertex_array *vxa):
    va(vxa)
{
    glGenBuffers(1, &buffer);
}


dake::gl::elements_array::~elements_array(void)
{
    glDeleteBuffers(1, &buffer);
}


void dake::gl::elements_array::format(int elements_per_vertex, GLenum type)
{
    epv = elements_per_vertex;
    switch (type) {
        case GL_UNSIGNED_BYTE:  bpv = epv * sizeof(GLubyte);  break;
        case GL_UNSIGNED_SHORT: bpv = epv * sizeof(GLushort); break;
        case GL_UNSIGNED_INT:   bpv = epv * sizeof(GLuint);   break;
        default:                throw std::invalid_argument("Unknown type given for elements_array::format");
    }

    t = type;
}


void dake::gl::elements_array::bind(void)
{
    if (dake::gl::cur_ea == this) {
        return;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    dake::gl::cur_ea = this;
}


void dake::gl::elements_array::data(void *ptr, size_t size, GLenum usage)
{
    if (size == static_cast<size_t>(-1)) {
        size = va->n * bpv;
    }

    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, ptr, usage);
}
