#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>


namespace dake
{

namespace gl
{

vertex_attrib *curr_vattr = nullptr;

}

}


dake::gl::vertex_attrib::vertex_attrib(vertex_array *vxa, GLuint a_id):
    attrib(a_id),
    va(vxa)
{
    glGenBuffers(1, &buffer);
}


dake::gl::vertex_attrib::~vertex_attrib(void)
{
    if (!buffer_reused) {
        glDeleteBuffers(1, &buffer);
    }
}


void dake::gl::vertex_attrib::bind(void)
{
    if (curr_vattr == this) {
        return;
    }

    va->bind();
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    curr_vattr = this;
}


void dake::gl::vertex_attrib::reuse_buffer(vertex_attrib *ova)
{
    if (ova == this) {
        throw std::invalid_argument("Cannot reuse own buffer");
    }

    if (!buffer_reused) {
        glDeleteBuffers(1, &buffer);
    }

    buffer = ova->buffer;
    buffer_reused = true;
}


void dake::gl::vertex_attrib::reuse_buffer(GLuint buffer_id)
{
    if (!buffer_reused) {
        glDeleteBuffers(1, &buffer);
    }

    buffer = buffer_id;
    buffer_reused = true;
}


#define TYPE(gl, real) case gl: bpv = epv * sizeof(real); break;

void dake::gl::vertex_attrib::format(int elements_per_vertex, GLenum type)
{
    epv = elements_per_vertex;
    t = type;

    switch (type) {
        TYPE(GL_FLOAT, float)
        TYPE(GL_INT, int32_t)
        TYPE(GL_DOUBLE, double)
        TYPE(GL_UNSIGNED_INT, uint32_t)
        TYPE(GL_UNSIGNED_INT64_ARB, uint64_t)
        default:
            throw std::invalid_argument("Unknown type given for vertex_attrib::format");
    }
}

#undef TYPE


void dake::gl::vertex_attrib::load(size_t stride, uintptr_t offset)
{
    bind();

    if ((t == GL_DOUBLE) || (t == GL_UNSIGNED_INT64_ARB)) {
        glVertexAttribLPointer(attrib, epv, t, stride, reinterpret_cast<const void *>(offset));
    } else if ((t == GL_INT) || (t == GL_UNSIGNED_INT)) {
        glVertexAttribIPointer(attrib, epv, t, stride, reinterpret_cast<const void *>(offset));
    } else {
        glVertexAttribPointer(attrib, epv, t, GL_FALSE, stride, reinterpret_cast<const void *>(offset));
    }
}


void dake::gl::vertex_attrib::data(const void *ptr, size_t size, GLenum usage, bool autoload)
{
    if (size == static_cast<size_t>(-1)) {
        size = va->n * bpv;
    }

    bind();
    glBufferData(GL_ARRAY_BUFFER, size, ptr, usage);

    if (autoload) {
        load();
    }
}


void *dake::gl::vertex_attrib::map(bool readable)
{
    bind();
    return glMapBuffer(GL_ARRAY_BUFFER, readable ? GL_READ_WRITE : GL_WRITE_ONLY);
}


void dake::gl::vertex_attrib::unmap(void)
{
    // checks are for noobs
    bind();
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
