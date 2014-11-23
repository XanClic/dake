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
static std::vector<const cubemap *> tmu_cubemap_bindings;

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
    if (img.compressed()) {
        glCompressedTexImage2D(target, 0, img.gl_format(), img.width(), img.height(), 0, img.byte_size(), img.data());
    } else {
        glTexImage2D(target, 0, img.gl_format(), img.width(), img.height(), 0, img.gl_format(), img.gl_type(), img.data());
    }
}


dake::gl::texture::texture(const char *name):
    tmu_index(0),
    fname(name)
{
    raw_init();

    dake::gl::image img(name);
    if (img.compressed()) {
        glCompressedTexImage2D(target, 0, img.gl_format(), img.width(), img.height(), 0, img.byte_size(), img.data());
    } else {
        glTexImage2D(target, 0, img.gl_format(), img.width(), img.height(), 0, img.gl_format(), img.gl_type(), img.data());
    }
}


dake::gl::texture::texture(const image &img):
    tmu_index(0),
    fname("[anon]")
{
    raw_init();

    if (img.compressed()) {
        glCompressedTexImage2D(target, 0, img.gl_format(), img.width(), img.height(), 0, img.byte_size(), img.data());
    } else {
        glTexImage2D(target, 0, img.gl_format(), img.width(), img.height(), 0, img.gl_format(), img.gl_type(), img.data());
    }
}


dake::gl::texture::texture(bool multisample):
    tmu_index(0),
    fname("[anon]"),
    multisampled(multisample)
{
    target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    raw_init();
}


dake::gl::texture::texture(const texture &orig, GLenum fmt):
    tmu_index(0),
    fname(orig.fname)
{
    if (!glext.has_extension(TEXTURE_VIEW)) {
        throw std::runtime_error("No texture view support");
    }

    glGenTextures(1, &tex_id);

    glTextureView(tex_id, target, orig.tex_id, fmt, 0, 1, 0, 1);
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
            glBindTexture(target, tex_id);
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
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
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


void dake::gl::texture::format(GLenum fmt, int w, int h, GLenum read_format, GLenum read_data_format, int spp)
{
    if (bl) {
        throw std::runtime_error("Cannot change format of a bindless texture");
    }

    bind();

    if (multisampled) {
        glTexImage2DMultisample(target, spp, fmt, w, h, false);
    } else if (spp == 1) {
        glTexImage2D(target, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
    } else {
        throw std::invalid_argument("Cannot use another sample per pixel count than 1 for non-multisample textures");
    }
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
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag_filter);
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
    glTexParameteri(target, GL_TEXTURE_WRAP_S, s_wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, t_wrap);
}


void dake::gl::texture::set_border_color(const dake::math::vec4 &color)
{
    if (bl) {
        throw std::runtime_error("Cannot change border color of a bindless texture");
    }

    bind();
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color);
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


    width  = w;
    height = h;
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


void dake::gl::array_texture::set_border_color(const dake::math::vec4 &color)
{
    if (bl) {
        throw std::runtime_error("Cannot change border color of a bindless array texture");
    }

    bind();
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, color);
}


void dake::gl::array_texture::load_layer(int layer, const dake::gl::image &img)
{
    if ((layer < 0) || (layer >= layers)) {
        throw std::invalid_argument("Array texture layer out of bounds");
    }

    bind(true);
    if (img.compressed()) {
        glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, img.width(), img.height(), 1, img.gl_format(), img.byte_size(), img.data());
    } else {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, img.width(), img.height(), 1, img.gl_format(), img.gl_type(), img.data());
    }
}


void dake::gl::array_texture::load_layer(int layer, const void *data, GLenum f, GLenum df)
{
    if ((layer < 0) || (layer >= layers)) {
        throw std::invalid_argument("Array texture layer out of bounds");
    }

    bind(true);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, f, df, data);
}


dake::gl::cubemap::cubemap(void):
    tmu_index(0)
{
    glGenTextures(1, &tex_id);

    bind();
    wrap(GL_CLAMP);
    filter(GL_LINEAR);
}


dake::gl::cubemap::~cubemap(void)
{
    if (bl && is_resident) {
        make_resident(false);
    }

    glDeleteTextures(1, &tex_id);
}


void dake::gl::cubemap::bind(bool force) const
{
    if (!bl || force) {
        if (static_cast<int>(tmu_cubemap_bindings.size()) < tmu_index + 1) {
            int old_size = tmu_cubemap_bindings.size();
            tmu_cubemap_bindings.resize(tmu_index + 1);

            for (int i = old_size; i <= tmu_index; i++) {
                tmu_cubemap_bindings[i] = nullptr;
            }
        }

        if (active_tmu != tmu_index) {
            glActiveTexture(GL_TEXTURE0 + tmu_index);
            active_tmu = tmu_index;
        }

        if (tmu_cubemap_bindings[tmu_index] != this) {
            glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
            tmu_cubemap_bindings[tmu_index] = this;
        }
    }
}


void dake::gl::cubemap::unbind(int tmu_index)
{
    if ((static_cast<int>(tmu_cubemap_bindings.size()) < tmu_index + 1) || !tmu_cubemap_bindings[tmu_index]) {
        return;
    }

    if (active_tmu != tmu_index) {
        glActiveTexture(GL_TEXTURE0 + tmu_index);
        active_tmu = tmu_index;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    tmu_cubemap_bindings[tmu_index] = nullptr;
}


void dake::gl::cubemap::make_bindless(bool initially_resident)
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


void dake::gl::cubemap::make_resident(bool state)
{
    if (!bl) {
        throw std::runtime_error("Cannot set residency state of non-bindless cube map");
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


void dake::gl::cubemap::format(GLenum fmt, int w, int h, GLenum read_format, GLenum read_data_format)
{
    if (bl) {
        throw std::runtime_error("Cannot change format of a bindless cube map");
    }

    bind();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);


    width  = w;
    height = h;
}


void dake::gl::cubemap::filter(GLenum f)
{
    filter(f, f);
}


void dake::gl::cubemap::filter(GLenum min_filter, GLenum mag_filter)
{
    if (bl) {
        throw std::runtime_error("Cannot change filtering of a bindless cube map");
    }

    bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter);
}


void dake::gl::cubemap::wrap(GLenum w)
{
    wrap(w, w);
}


void dake::gl::cubemap::wrap(GLenum s_wrap, GLenum t_wrap)
{
    if (bl) {
        throw std::runtime_error("Cannot change wrap mode of a bindless cube map");
    }

    bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, s_wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, t_wrap);
}


void dake::gl::cubemap::set_border_color(const dake::math::vec4 &color)
{
    if (bl) {
        throw std::runtime_error("Cannot change border color of a bindless cube map");
    }

    bind();
    glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, color);
}


void dake::gl::cubemap::load_layer(layer l, const dake::gl::image &img)
{
    bind(true);
    if (img.compressed()) {
        glCompressedTexSubImage2D(l, 0, 0, 0, img.width(), img.height(), img.gl_format(), img.byte_size(), img.data());
    } else {
        glTexSubImage2D(l, 0, 0, 0, img.width(), img.height(), img.gl_format(), img.gl_type(), img.data());
    }
}


void dake::gl::cubemap::load_layer(layer l, const void *data, GLenum f, GLenum df)
{
    bind(true);
    glTexSubImage2D(l, 0, 0, 0, width, height, f, df, data);
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
