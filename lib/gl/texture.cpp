#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <stdexcept>
#include <string>

#include <png.h>

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


bool test_png(FILE *fp)
{
    uint8_t hdr[8];
    fread(hdr, 1, 8, fp);
    rewind(fp);

    return png_sig_cmp(hdr, 0, 8) == 0;
}

uint8_t *load_png(FILE *fp, int *width, int *height)
{
    // lol longjmp

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        throw std::runtime_error("Could not create PNG read struct");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        throw std::runtime_error("Could not create PNG info struct");
    }

    png_infop info_end = png_create_info_struct(png_ptr);
    if (!info_end) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        throw std::runtime_error("Could not create PNG end info struct");
    }

    png_init_io(png_ptr, fp);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, nullptr);

    int depth, fmt;
    uint32_t w, h;
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &depth, &fmt, nullptr, nullptr, nullptr);

    *width = w;
    *height = h;

    if ((fmt != PNG_COLOR_TYPE_GRAY) &&
        (fmt != PNG_COLOR_TYPE_GRAY_ALPHA) &&
        (fmt != PNG_COLOR_TYPE_PALETTE) &&
        (fmt != PNG_COLOR_TYPE_RGB) &&
        (fmt != PNG_COLOR_TYPE_RGB_ALPHA))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &info_end);
        throw std::runtime_error("Unknown PNG color format");
    }

    png_bytep *rows = png_get_rows(png_ptr, info_ptr);

    png_colorp palette;
    int palette_entries = 0;
    if (fmt == PNG_COLOR_TYPE_PALETTE) {
        png_get_PLTE(png_ptr, info_ptr, &palette, &palette_entries);
    }

    uint8_t *output = new uint8_t[w * h * 4];
    uint32_t ofs = 0;
    for (uint32_t y = 0; y < h; y++) {
        if (fmt == PNG_COLOR_TYPE_RGB_ALPHA) {
            memcpy(&output[ofs], rows[y], w * 4);
            ofs += w * 4;
        } else {
            uint32_t rofs = 0;
            for (uint32_t x = 0; x < w; x++) {
                switch (fmt) {
                    case PNG_COLOR_TYPE_GRAY:
                        output[ofs++] = rows[y][rofs];
                        output[ofs++] = rows[y][rofs];
                        output[ofs++] = rows[y][rofs++];
                        output[ofs++] = 0xff;
                        break;

                    case PNG_COLOR_TYPE_GRAY_ALPHA:
                        output[ofs++] = rows[y][rofs];
                        output[ofs++] = rows[y][rofs];
                        output[ofs++] = rows[y][rofs++];
                        output[ofs++] = rows[y][rofs++];
                        break;

                    case PNG_COLOR_TYPE_PALETTE:
                        output[ofs++] = palette[rows[y][rofs  ]].red;
                        output[ofs++] = palette[rows[y][rofs  ]].green;
                        output[ofs++] = palette[rows[y][rofs++]].blue;
                        output[ofs++] = 0xff;
                        break;

                    case PNG_COLOR_TYPE_RGB:
                        output[ofs++] = rows[y][rofs++];
                        output[ofs++] = rows[y][rofs++];
                        output[ofs++] = rows[y][rofs++];
                        output[ofs++] = 0xff;
                        break;
                }
            }
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &info_end);

    return output;
}


struct image_format {
    const char *name;

    bool (*test)(FILE *fp);
    uint8_t *(*load)(FILE *fp, int *width, int *height);
};

image_format formats[] = {
    {
        "png",
        test_png,
        load_png
    },
};


dake::gl::texture::texture(const std::string &name):
    tmu_index(0),
    fname(name)
{
    FILE *fp = fopen(name.c_str(), "rb");
    if (!fp) {
        throw std::runtime_error("Could not load texture from " + name + ": " + strerror(errno));
    }

    for (const image_format &fmt: formats) {
        if (fmt.test(fp)) {
            int w, h;
            uint8_t *buffer;
            try {
                buffer = fmt.load(fp, &w, &h);
            } catch (std::exception e) {
                throw std::runtime_error("Could not load texture from " + name + ": " + e.what());
            }

            fclose(fp);

            raw_init();

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

            delete[] buffer;

            return;
        }
    }

    fclose(fp);

    std::string format_list;

    int format_count = static_cast<int>(sizeof(formats) / sizeof(formats[0]));
    for (int i = 0; i < format_count; i++) {
        format_list += formats[i].name;

        if (i < format_count - 1) {
            format_list += ", ";
        }
    }

    throw std::runtime_error("Could not load texture from " + name + ": Unsupported format (supported formats: " + format_list + ")");
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
    glActiveTexture(GL_TEXTURE0 + tmu_index);
    glBindTexture(GL_TEXTURE_2D, tex_id);
}


void dake::gl::texture::format(GLenum fmt, int w, int h, GLenum read_format, GLenum read_data_format)
{
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, read_format, read_data_format, nullptr);
}


void dake::gl::texture::filter(GLenum f)
{
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
