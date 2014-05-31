#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>


dake::gl::vertex_attrib::vertex_attrib(vertex_array *vxa, GLuint a_id):
    attrib(a_id),
    va(vxa)
{
    glGenBuffers(1, &buffer);
}


dake::gl::vertex_attrib::~vertex_attrib(void)
{
    if (!buffer_reused)
        glDeleteBuffers(1, &buffer);
}


void dake::gl::vertex_attrib::reuse_buffer(vertex_attrib *va)
{
    if (va == this)
        throw std::invalid_argument("Cannot reuse own buffer");

    if (!buffer_reused)
        glDeleteBuffers(1, &buffer);

    buffer = va->buffer;
    buffer_reused = true;
}


#define TYPE(gl, real) case gl: bpv = epv * sizeof(real); break;

void dake::gl::vertex_attrib::format(int elements_per_vertex, GLenum type)
{
    epv = elements_per_vertex;
    t = type;

    switch (type)
    {
        TYPE(GL_FLOAT, float)
        TYPE(GL_INT, int)
        TYPE(GL_DOUBLE, double)
        default:
            throw std::invalid_argument("Unknown type given for vertex_attrib::format");
    }
}

#undef TYPE


void dake::gl::vertex_attrib::load(size_t stride, uintptr_t offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(attrib, epv, t, GL_FALSE, stride, reinterpret_cast<const void *>(offset));
}


void dake::gl::vertex_attrib::data(void *ptr, size_t size, GLenum usage)
{
    if (size == (size_t)-1)
        size = va->n * bpv;

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, ptr, usage);
    load();
}