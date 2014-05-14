#ifndef DAKE__GL__TEXTURE_HPP
#define DAKE__GL__TEXTURE_HPP

#include <list>
#include <string>
#include <QtOpenGL>


namespace dake
{

namespace gl
{

class texture
{
    private:
        GLuint tex_id;
        int tmu;
        std::string fname;

        void raw_init(void);

    public:
        texture(void);
        texture(const std::string &name);
        ~texture(void);

        void bind(void) const;

        static void unbind(int tmu = 0) { glActiveTexture(GL_TEXTURE0 + tmu); glBindTexture(GL_TEXTURE_2D, 0); }

        void format(GLenum format, int w, int h, GLenum read_format = GL_RGB, GLenum read_data_format = GL_UNSIGNED_BYTE);
        void filter(GLenum filter);
        void set_tmu(int tmu) { this->tmu = tmu; }

        const std::string &get_fname(void) const { return fname; }
        GLuint get_glid(void) const { return tex_id; }
        int get_tmu(void) const { return tmu; }
};


class texture_manager
{
    private:
        std::list<texture *> textures;

    public:
        ~texture_manager(void);

        const texture *find_texture(const std::string &name);

        static texture_manager &instance(void)
        {
            static texture_manager *texman = NULL;
            if (!texman) texman = new texture_manager;
            return *texman;
        }
};

}

}

#endif
