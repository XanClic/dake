#include <cstdlib>
#include <cstdio>
#include <list>
#include <string>
#include <QtOpenGL>

#include <dake/gl/texture.hpp>


void dake::gl::texture::raw_init(void)
{
    glGenTextures(1, &tex_id);

    glActiveTexture(GL_TEXTURE0 + tmu);
    glEnable(GL_TEXTURE_2D);
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    filter(GL_LINEAR);
}


dake::gl::texture::texture(const std::string &name):
    tmu(0),
    fname(name)
{
    /*
    cgv::data::data_format df;
    cgv::media::image::image_reader ir(df);

    cgv::data::data_view dv;
    if (!ir.read_image(name, dv))
    {
        fprintf(stderr, "Could not load image: %s\n", ir.get_last_error().c_str());
        throw 23;
    }
    */

    raw_init();

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dv.get_format()->get_width(), dv.get_format()->get_width(), 0, GL_RGB, GL_UNSIGNED_BYTE, dv.get_ptr(0));
}


dake::gl::texture::texture(void):
    tmu(0),
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
    glActiveTexture(GL_TEXTURE0 + tmu);
    glBindTexture(GL_TEXTURE_2D, tex_id);
}


void dake::gl::texture::format(GLenum format, int w, int h, GLenum read_format, GLenum read_data_format)
{
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, read_format, read_data_format, NULL);
}


void dake::gl::texture::filter(GLenum filter)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}


dake::gl::texture_manager::~texture_manager(void)
{
    for (std::list<texture *>::iterator i = textures.begin(); i != textures.end(); i++)
        delete *i;
}


const dake::gl::texture *dake::gl::texture_manager::find_texture(const std::string &name)
{
    for (std::list<dake::gl::texture *>::iterator i = textures.begin(); i != textures.end(); i++)
        if (name == (*i)->get_fname())
            return *i;

    texture *nt = new dake::gl::texture(name);
    textures.push_back(nt);
    return nt;
}
