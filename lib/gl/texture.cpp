#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <stdexcept>
#include <string>

#include <dake/gl/find_resource.hpp>
#include <dake/gl/gl.hpp>
#include <dake/gl/texture.hpp>


void dake::gl::texture::raw_init(void)
{
    glGenTextures(1, &tex_id);

    glActiveTexture(GL_TEXTURE0 + tmu_index);
    glEnable(GL_TEXTURE_2D);
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    filter(GL_LINEAR);
}


dake::gl::texture::texture(const std::string &name):
    tmu_index(0),
    fname(name)
{
    raw_init();

    dake::gl::image img(name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, img.gl_format(), img.gl_type(), img.data());
}


dake::gl::texture::texture(const image &img):
    tmu_index(0),
    fname("[anon]")
{
    raw_init();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, img.gl_format(), img.gl_type(), img.data());
}


dake::gl::texture::texture(void):
    tmu_index(0),
    fname("[anon]")
{
    raw_init();
}


dake::gl::texture::~texture(void)
{
    glDeleteTextures(1, &tex_id);
}


void dake::gl::texture::bind(void) const
{
    if (!bl) {
        glActiveTexture(GL_TEXTURE0 + tmu_index);
        glBindTexture(GL_TEXTURE_2D, tex_id);
    }
}


void dake::gl::texture::make_bindless(bool initially_resident)
{
    if (bl) {
        return;
    }

    if (!glext.has_bindless_textures()) {
        throw std::runtime_error("No bindless texture support");
    }

    bl_handle = glGetTextureHandleARB(tex_id);
    bl = true;

    if (initially_resident) {
        make_resident(true);
    } else {
        is_resident = false;
    }
}


void dake::gl::texture::make_resident(bool state)
{
    if (!bl) {
        throw std::runtime_error("Cannot set residency state of non-bindless texture");
    }

    if (is_resident == state) {
        return;
    }

    if (state) {
        glMakeTextureHandleResidentARB(bl_handle);
    } else {
        glMakeTextureHandleNonResidentARB(bl_handle);
    }

    is_resident = state;
}


void dake::gl::texture::format(GLenum fmt, int w, int h, GLenum read_format, GLenum read_data_format)
{
    if (bl) {
        throw std::runtime_error("Cannot change format of a bindless texture");
    }

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
}


void dake::gl::texture::filter(GLenum f)
{
    if (bl) {
        throw std::runtime_error("Cannot change filtering of a bindless texture");
    }

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, f);
}


dake::gl::texture_manager::~texture_manager(void)
{
    for (texture *t: textures) {
        delete t;
    }
}


const dake::gl::texture *dake::gl::texture_manager::find_texture(const std::string &name)
{
    for (texture *t: textures) {
        if (name == t->get_fname()) {
            return t;
        }
    }

    texture *nt = new dake::gl::texture(name);
    textures.push_back(nt);
    return nt;
}
