#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

#include <dake/gl/find_resource.hpp>
#include <dake/gl/gl.hpp>
#include <dake/gl/texture.hpp>


namespace dake
{

namespace gl
{

static std::vector<const texture *> tmu_bindings;
static std::vector<const array_texture *> tmu_array_bindings;

static int active_tmu;

}

}


void dake::gl::texture::raw_init(void)
{
    glGenTextures(1, &tex_id);

    bind();
    wrap(GL_CLAMP);
    filter(GL_LINEAR);
}


dake::gl::texture::texture(const std::string &name):
    tmu_index(0),
    fname(name)
{
    raw_init();

    dake::gl::image img(name);
    glTexImage2D(GL_TEXTURE_2D, 0, img.gl_format(), img.width(), img.height(), 0, img.gl_format(), img.gl_type(), img.data());
}


dake::gl::texture::texture(const image &img):
    tmu_index(0),
    fname("[anon]")
{
    raw_init();

    glTexImage2D(GL_TEXTURE_2D, 0, img.gl_format(), img.width(), img.height(), 0, img.gl_format(), img.gl_type(), img.data());
}


dake::gl::texture::texture(void):
    tmu_index(0),
    fname("[anon]")
{
    raw_init();
}


dake::gl::texture::~texture(void)
{
    if (bl && is_resident) {
        make_resident(false);
    }

    glDeleteTextures(1, &tex_id);
}


void dake::gl::texture::bind(bool force) const
{
    if (!bl || force) {
        if (static_cast<int>(tmu_bindings.size()) < tmu_index + 1) {
            int old_size = tmu_bindings.size();
            tmu_bindings.resize(tmu_index + 1);

            for (int i = old_size; i <= tmu_index; i++) {
                tmu_bindings[i] = nullptr;
            }
        }

        if (active_tmu != tmu_index) {
            glActiveTexture(GL_TEXTURE0 + tmu_index);
            active_tmu = tmu_index;
        }

        if (tmu_bindings[tmu_index] != this) {
            glBindTexture(GL_TEXTURE_2D, tex_id);
            tmu_bindings[tmu_index] = this;
        }
    }
}


void dake::gl::texture::unbind(int tmu_index)
{
    if ((static_cast<int>(tmu_bindings.size()) < tmu_index + 1) || !tmu_bindings[tmu_index]) {
        return;
    }

    if (active_tmu != tmu_index) {
        glActiveTexture(GL_TEXTURE0 + tmu_index);
        active_tmu = tmu_index;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    tmu_bindings[tmu_index] = nullptr;
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
    filter(f, f);
}


void dake::gl::texture::filter(GLenum min_filter, GLenum mag_filter)
{
    if (bl) {
        throw std::runtime_error("Cannot change filtering of a bindless texture");
    }

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
}


void dake::gl::texture::wrap(GLenum w)
{
    wrap(w, w);
}


void dake::gl::texture::wrap(GLenum s_wrap, GLenum t_wrap)
{
    if (bl) {
        throw std::runtime_error("Cannot change wrap mode of a bindless texture");
    }

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t_wrap);
}


dake::gl::array_texture::array_texture(void):
    tmu_index(0)
{
    glGenTextures(1, &tex_id);

    bind();
    wrap(GL_CLAMP);
    filter(GL_LINEAR);
}


dake::gl::array_texture::~array_texture(void)
{
    if (bl && is_resident) {
        make_resident(false);
    }

    glDeleteTextures(1, &tex_id);
}


void dake::gl::array_texture::bind(bool force) const
{
    if (!bl || force) {
        if (static_cast<int>(tmu_array_bindings.size()) < tmu_index + 1) {
            int old_size = tmu_array_bindings.size();
            tmu_array_bindings.resize(tmu_index + 1);

            for (int i = old_size; i <= tmu_index; i++) {
                tmu_array_bindings[i] = nullptr;
            }
        }

        if (active_tmu != tmu_index) {
            glActiveTexture(GL_TEXTURE0 + tmu_index);
            active_tmu = tmu_index;
        }

        if (tmu_array_bindings[tmu_index] != this) {
            glBindTexture(GL_TEXTURE_2D_ARRAY, tex_id);
            tmu_array_bindings[tmu_index] = this;
        }
    }
}


void dake::gl::array_texture::unbind(int tmu_index)
{
    if ((static_cast<int>(tmu_array_bindings.size()) < tmu_index + 1) || !tmu_array_bindings[tmu_index]) {
        return;
    }

    if (active_tmu != tmu_index) {
        glActiveTexture(GL_TEXTURE0 + tmu_index);
        active_tmu = tmu_index;
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    tmu_array_bindings[tmu_index] = nullptr;
}


void dake::gl::array_texture::make_bindless(bool initially_resident)
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


void dake::gl::array_texture::make_resident(bool state)
{
    if (!bl) {
        throw std::runtime_error("Cannot set residency state of non-bindless array texture");
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


void dake::gl::array_texture::format(GLenum fmt, int w, int h, int l, GLenum read_format, GLenum read_data_format)
{
    if (bl) {
        throw std::runtime_error("Cannot change format of a bindless array texture");
    }

    bind();
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, fmt, w, h, l, 0, read_format, read_data_format, nullptr);

    layers = l;
}


void dake::gl::array_texture::filter(GLenum f)
{
    filter(f, f);
}


void dake::gl::array_texture::filter(GLenum min_filter, GLenum mag_filter)
{
    if (bl) {
        throw std::runtime_error("Cannot change filtering of a bindless array texture");
    }

    bind();
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, mag_filter);
}


void dake::gl::array_texture::wrap(GLenum w)
{
    wrap(w, w);
}


void dake::gl::array_texture::wrap(GLenum s_wrap, GLenum t_wrap)
{
    if (bl) {
        throw std::runtime_error("Cannot change wrap mode of a bindless array texture");
    }

    bind();
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, s_wrap);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, t_wrap);
}



void dake::gl::array_texture::load_layer(int layer, const dake::gl::image &img)
{
    if ((layer < 0) || (layer >= layers)) {
        throw std::invalid_argument("Array texture layer out of bounds");
    }

    bind(true);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, img.width(), img.height(), 1, img.gl_format(), img.gl_type(), img.data());
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
