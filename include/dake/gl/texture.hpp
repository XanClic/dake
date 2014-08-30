#ifndef DAKE__GL__TEXTURE_HPP
#define DAKE__GL__TEXTURE_HPP

#include <vector>
#include <string>

#include "dake/gl/gl.hpp"


namespace dake
{

namespace gl
{

class image {
    public:
        enum channel_format {
            LINEAR_UINT8
        };

    private:
        void *d = nullptr;
        channel_format fmt;
        int w, h, cc;

        void load(const void *buffer, size_t length, const std::string &name);

    public:
        image(const std::string &file);
        image(const void *buffer, size_t length);
        ~image(void);

        int width(void) const { return w; }
        int height(void) const { return h; }
        int channels(void) const { return cc; }
        channel_format format(void) const { return fmt; }
        const void *data(void) const { return d; }

        GLenum gl_format(void) const;
        GLenum gl_type(void) const;
};


class texture {
    private:
        GLuint tex_id;
        int tmu_index;
        std::string fname;
        bool bl = false, is_resident = false;
        uint64_t bl_handle;

        void raw_init(void);

    public:
        texture(void);
        texture(const std::string &name);
        texture(const image &img);
        texture(const texture &orig, GLenum format); // creates a texture view
        ~texture(void);

        void bind(bool force = false) const;
        static void unbind(int tmu = 0);

        bool bindless(void) const { return bl; }
        void make_bindless(bool resident = true);
        uint64_t handle(void) const { return bl_handle; }
        bool resident(void) const { return is_resident; }
        void make_resident(bool state);

        void format(GLenum format, int w, int h, GLenum read_format = GL_RGB, GLenum read_data_format = GL_UNSIGNED_BYTE);
        void filter(GLenum filter);
        void filter(GLenum min_filter, GLenum mag_filter);
        void wrap(GLenum wrap);
        void wrap(GLenum s_wrap, GLenum t_wrap);
        void set_tmu(int tmui) { tmu_index = tmui; }

        const std::string &get_fname(void) const { return fname; }
        GLuint get_glid(void) const { return tex_id; }
        int get_tmu(void) const { return tmu_index; }

        int &tmu(void) { return tmu_index; }
        int tmu(void) const { return tmu_index; }
        GLuint glid(void) const { return tex_id; }
};


class array_texture {
    private:
        GLuint tex_id;
        int tmu_index;
        bool bl = false, is_resident = false;
        uint64_t bl_handle;
        int layers;

    public:
        array_texture(void);
        ~array_texture(void);

        void bind(bool force = false) const;
        static void unbind(int tmu = 0);

        bool bindless(void) const { return bl; }
        void make_bindless(bool resident = true);
        uint64_t handle(void) const { return bl_handle; }
        bool resident(void) const { return is_resident; }
        void make_resident(bool state);

        void format(GLenum format, int w, int h, int layers, GLenum read_format = GL_RGB, GLenum read_data_format = GL_UNSIGNED_BYTE);
        void filter(GLenum filter);
        void filter(GLenum min_filter, GLenum mag_filter);
        void wrap(GLenum wrap);
        void wrap(GLenum s_wrap, GLenum t_wrap);

        void load_layer(int layer, const image &img);

        int &tmu(void) { return tmu_index; }
        int tmu(void) const { return tmu_index; }
        GLuint glid(void) const { return tex_id; }
};


class texture_manager
{
    private:
        std::vector<texture *> textures;

    public:
        ~texture_manager(void);

        const texture *find_texture(const std::string &name);

        static texture_manager &instance(void)
        {
            static texture_manager *texman = nullptr;
            if (!texman) texman = new texture_manager;
            return *texman;
        }
};

}

}

#endif
