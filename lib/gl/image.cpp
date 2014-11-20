#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

#include <png.h>
#include <jpeglib.h>

#include <dake/gl/find_resource.hpp>
#include <dake/gl/gl.hpp>
#include <dake/gl/texture.hpp>


bool test_png(const void *buffer, size_t length)
{
    return png_sig_cmp(static_cast<png_const_bytep>(buffer), 0, length < 8 ? length : 8) == 0;
}


void *load_png(const void *buffer, size_t length, int *width, int *height, int *channels, dake::gl::image::channel_format *format)
{
    // lol longjmp

    FILE *fp = fmemopen(const_cast<void *>(buffer), length, "rb");
    if (!fp) {
        throw std::runtime_error("Could not open PNG buffer for reading");
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        throw std::runtime_error("Could not create PNG read struct");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Could not create PNG info struct");
    }

    png_infop info_end = png_create_info_struct(png_ptr);
    if (!info_end) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Could not create PNG end info struct");
    }

    png_init_io(png_ptr, fp);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, nullptr);

    int depth, fmt;
    uint32_t w, h;
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &depth, &fmt, nullptr, nullptr, nullptr);

    *width = w;
    *height = h;
    *format = dake::gl::image::LINEAR_UINT8;

    switch (fmt) {
        case PNG_COLOR_TYPE_GRAY:       *channels = 1; break;
        case PNG_COLOR_TYPE_GRAY_ALPHA: *channels = 2; break;
        case PNG_COLOR_TYPE_PALETTE:
        case PNG_COLOR_TYPE_RGB:        *channels = 3; break;
        case PNG_COLOR_TYPE_RGB_ALPHA:  *channels = 4; break;
        default:
            png_destroy_read_struct(&png_ptr, &info_ptr, &info_end);
            fclose(fp);
            throw std::runtime_error("Unknown PNG color format");
    }

    png_bytep *rows = png_get_rows(png_ptr, info_ptr);

    png_colorp palette;
    int palette_entries = 0;
    if (fmt == PNG_COLOR_TYPE_PALETTE) {
        png_get_PLTE(png_ptr, info_ptr, &palette, &palette_entries);
    }

    uint8_t *output = new uint8_t[w * h * *channels];
    uint32_t ofs = 0;
    for (uint32_t y = 0; y < h; y++) {
        if (fmt == PNG_COLOR_TYPE_PALETTE) {
            uint32_t rofs = 0;
            for (uint32_t x = 0; x < w; x++) {
                output[ofs++] = palette[rows[y][rofs  ]].red;
                output[ofs++] = palette[rows[y][rofs  ]].green;
                output[ofs++] = palette[rows[y][rofs++]].blue;
            }
        } else {
            memcpy(&output[ofs], rows[y], w * *channels);
            ofs += w * *channels;
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &info_end);
    fclose(fp);

    return output;
}


bool test_jpg(const void *buffer, size_t length)
{
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jpg_err;
    FILE *fp = fmemopen(const_cast<void *>(buffer), length, "rb");
    if (!fp) {
        return false;
    }

    cinfo.err = jpeg_std_error(&jpg_err);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fp);

    bool success = jpeg_read_header(&cinfo, true);

    jpeg_destroy_decompress(&cinfo);
    fclose(fp);

    return success;
}


void *load_jpg(const void *buffer, size_t length, int *width, int *height, int *channels, dake::gl::image::channel_format *format)
{
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jpg_err;
    FILE *fp = fmemopen(const_cast<void *>(buffer), length, "rb");
    if (!fp) {
        throw std::runtime_error("Could not open JPEG buffer for reading");
    }

    cinfo.err = jpeg_std_error(&jpg_err);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fp);

    jpeg_read_header(&cinfo, true);
    jpeg_start_decompress(&cinfo);

    if ((cinfo.output_components < 1) || (cinfo.output_components > 4)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        throw std::runtime_error("Invalid number of JPEG color channels");
    }

    *width    = cinfo.output_width;
    *height   = cinfo.output_height;
    *channels = cinfo.output_components;
    *format   = dake::gl::image::LINEAR_UINT8;

    uint8_t *output = new uint8_t[*width * *height * *channels];
    uint8_t *target = output;
    while (static_cast<int>(cinfo.output_scanline) < *height) {
        jpeg_read_scanlines(&cinfo, &target, 1);
        target += *width * *channels;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);

    return output;
}


struct image_format {
    const char *name;

    bool (*test)(const void *buffer, size_t length);
    void *(*load)(const void *buffer, size_t length, int *width, int *height, int *channels, dake::gl::image::channel_format *fmt);
};


static const image_format formats[] = {
    {
        "png",
        test_png,
        load_png
    },

    {
        "jpg",
        test_jpg,
        load_jpg
    },
};


dake::gl::image::image(const std::string &file)
{
    FILE *fp = fopen(dake::gl::find_resource_filename(file).c_str(), "rb");
    if (!fp) {
        throw std::runtime_error("Could not load image from " + file + ": " + strerror(errno));
    }

    fseek(fp, 0, SEEK_END);
    size_t lof = ftell(fp);
    rewind(fp);

    void *buffer = malloc(lof);
    fread(buffer, 1, lof, fp);

    fclose(fp);

    try {
        load(buffer, lof, file);
    } catch (...) {
        free(buffer);
        throw;
    }

    free(buffer);
}


dake::gl::image::image(const void *buffer, size_t length)
{
    char *name = new char[2 + sizeof(buffer) * 2 + 1];
    snprintf(name, 2 + sizeof(buffer) * 2 + 1, "%p", buffer);

    try {
        load(buffer, length, name);
    } catch (...) {
        free(name);
        throw;
    }

    free(name);
}


dake::gl::image::image(const dake::gl::image &i1, const dake::gl::image &i2)
{
    if (i1.channels() + i2.channels() > 4) {
        throw std::invalid_argument("Images have too many channels");
    }

    if ((i1.width() != i2.width()) || (i1.height() != i2.height())) {
        throw std::invalid_argument("Images do not have the same size");
    }

    assert(i1.format() == LINEAR_UINT8);
    assert(i2.format() == LINEAR_UINT8);

    w = i1.width();
    h = i1.height();
    cc = i1.channels() + i2.channels();
    fmt = LINEAR_UINT8;

    d = new uint8_t[w * h * cc];

    const uint8_t *s1 = static_cast<const uint8_t *>(i1.data());
    const uint8_t *s2 = static_cast<const uint8_t *>(i2.data());

    uint8_t *dst = static_cast<uint8_t *>(d);

    for (int i = 0; i < w * h; i++) {
        for (int c = 0; c < i1.channels(); c++) {
            *(dst++) = *(s1++);
        }
        for (int c = 0; c < i2.channels(); c++) {
            *(dst++) = *(s2++);
        }
    }
}


dake::gl::image::~image(void)
{
    free(d);
}


void dake::gl::image::load(const void *buffer, size_t length, const std::string &name)
{
    for (const image_format &f: formats) {
        if (f.test(buffer, length)) {
            try {
                d = f.load(buffer, length, &w, &h, &cc, &fmt);
            } catch (const std::exception &e) {
                throw std::runtime_error("Could not load image from " + name + ": " + e.what());
            }

            return;
        }
    }

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


static const GLenum gl_formats[] = {
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA
};


GLenum dake::gl::image::gl_format(void) const
{
    return gl_formats[cc - 1];
}


static const GLenum gl_types[] = {
    GL_UNSIGNED_BYTE, // LINEAR_UINT8
};


GLenum dake::gl::image::gl_type(void) const
{
    return gl_types[fmt];
}
