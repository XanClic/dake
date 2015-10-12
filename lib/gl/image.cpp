#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

extern "C" {
#include <png.h>
#include <jpeglib.h>
#include <txc_dxtn.h>
}

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

    uint8_t *output = new uint8_t[h * ((w * *channels + 3) & ~3u)];
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
        ofs = (ofs + 3) & ~3u;
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &info_end);
    fclose(fp);

    return output;
}


struct bitmap_file_header {
    uint16_t bfType;
    uint32_t bfSize;
    uint32_t bfReserved;
    uint32_t bfOffBits;
} __attribute__((packed));

struct bitmap_info_header {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} __attribute__((packed));

bool test_bmp(const void *buffer, size_t length)
{
    return length >= 54 && !strncmp((const char *)buffer, "BM", 2);
}


void *load_bmp(const void *buffer, size_t length, int *width, int *height, int *channels, dake::gl::image::channel_format *format)
{
    const bitmap_file_header *bfh = static_cast<const bitmap_file_header *>(buffer);
    const bitmap_info_header *bih = reinterpret_cast<const bitmap_info_header *>(bfh + 1);

    if (bih->biCompression) {
        throw std::runtime_error("BMP is compressed");
    }

    if (bih->biBitCount != 1 && bih->biBitCount != 4 && bih->biBitCount != 8 &&
        bih->biBitCount != 16 && bih->biBitCount != 24 && bih->biBitCount != 32)
    {
        throw std::runtime_error("Invalid BMP bit count");
    }

    *width  = bih->biWidth;
    *height = abs(bih->biHeight);
    if (bih->biBitCount < 32) {
        *channels = 3;
    } else {
        *channels = 4;
    }

    int pal_entries = -1;
    if (bih->biBitCount <= 8) {
        pal_entries = bih->biClrUsed ? bih->biClrUsed : 1 << bih->biBitCount;
        if (pal_entries > 1 << bih->biBitCount) {
            pal_entries = 1 << bih->biBitCount;
        }
    }

    const uint8_t *pal = nullptr;
    if (pal_entries >= 0) {
        pal = reinterpret_cast<const uint8_t *>(bih + 1);
    }

    *format = dake::gl::image::channel_format::LINEAR_UINT8;
    uint8_t *output = new uint8_t[*height * ((*width * *channels + 3) & ~3u)];
    const uint8_t *input = static_cast<const uint8_t *>(buffer) + bfh->bfOffBits;

    int total = *width * *height * *channels;
    int i = 0, o = 0;
    int scanline = ((*width * bih->biBitCount + 7) / 8 + 3) & ~3u;

    for (int y = 0; y < *height; y++) {
        if (bih->biHeight < 0) {
            i = y * scanline;
        } else {
            i = (*height - y - 1) * scanline;
        }
        o = (o + 3) & ~3u;

        for (int x = 0; x < *width; x++) {
            if (pal) {
                if (length - i < 1) {
                    throw std::runtime_error("Unexpected end of BMP");
                }
                int to_read = 8 / bih->biBitCount;
                while (o < total && to_read--) {
                    int pi = input[i];

                    pi >>= to_read * bih->biBitCount;
                    pi &= (1 << bih->biBitCount) - 1;
                    if (pi >= pal_entries) {
                        throw std::runtime_error("BMP palette index out of bounds");
                    }

                    const uint8_t *pe = &pal[pi];
                    output[o++] = pe[2];
                    output[o++] = pe[1];
                    output[o++] = pe[0];

                    if (to_read) {
                        if (++x == *width) {
                            --x;
                            break;
                        }
                    }
                }
            } else if (bih->biBitCount == 16) {
                uint16_t val;
                if (length - i < 2) {
                    throw std::runtime_error("Unexpected end of BMP");
                }
                val = input[i++];
                val |= input[i++] << 8;
                output[o++] = (val >> 10) & 0x1f;
                output[o++] = (val >>  5) & 0x1f;
                output[o++] =  val        & 0x1f;
            } else {
                assert(bih->biBitCount == 24 || bih->biBitCount == 32);
                if (length - i < 4) {
                    throw std::runtime_error("Unexpected end of BMP");
                }
                if (bih->biBitCount == 32) {
                    output[o++] = input[i + 3];
                }
                output[o++] = input[i + 2];
                output[o++] = input[i + 1];
                output[o++] = input[i + 0];
                i += bih->biBitCount / 8;
            }
        }
    }

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

    uint8_t *output = new uint8_t[*height * ((*width * *channels + 3) & ~3u)];
    uint8_t *target = output;
    while (static_cast<int>(cinfo.output_scanline) < *height) {
        jpeg_read_scanlines(&cinfo, &target, 1);
        target += (*width * *channels + 3) & ~3u;
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
        "bmp",
        test_bmp,
        load_bmp
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
    if (fread(buffer, 1, lof, fp) < lof) {
        const char *err = strerror(errno);
        fclose(fp);
        free(buffer);
        throw std::runtime_error("Failed to read image from " + file + ": "
                                 + err);
    }

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
        delete[] name;
        throw;
    }

    delete[] name;
}


dake::gl::image::image(const dake::gl::image &i1, const dake::gl::image &i2)
{
    if (i1.channels() + i2.channels() > 4) {
        throw std::invalid_argument("Images have too many channels");
    }

    if ((i1.width() != i2.width()) || (i1.height() != i2.height())) {
        throw std::invalid_argument("Images do not have the same size");
    }

    if ((i1.format() != LINEAR_UINT8) || (i2.format() != LINEAR_UINT8)) {
        throw std::invalid_argument("Images are not in linear uint8 format");
    }

    w = i1.width();
    h = i1.height();
    cc = i1.channels() + i2.channels();
    fmt = LINEAR_UINT8;

    bsz = w * h * cc;

    d = new uint8_t[bsz];

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


static const GLenum gl_types[] = {
    GL_UNSIGNED_BYTE,                 // LINEAR_UINT8
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,  // COMPRESSED_S3TC_DXT1
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, // COMPRESSED_S3TC_DXT1_ALPHA
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, // COMPRESSED_S3TC_DXT3
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, // COMPRESSED_S3TC_DXT5
};


dake::gl::image::image(const dake::gl::image &input, channel_format new_format, int new_channels)
{
    if (!new_channels) {
        new_channels = input.channels();
    }

    switch (new_format) {
        case LINEAR_UINT8:
            bsz = input.width() * input.height() * new_channels;
            break;

        case COMPRESSED_S3TC_DXT1:
        case COMPRESSED_S3TC_DXT1_ALPHA:
            bsz = ((input.width() + 3) / 4) * ((input.height() + 3) / 4) * 8;
            break;

        case COMPRESSED_S3TC_DXT3:
        case COMPRESSED_S3TC_DXT5:
            bsz = ((input.width() + 3) / 4) * ((input.height() + 3) / 4) * 16;
            break;

        default:
            abort();
    }

    if (input.format() == new_format) {
        if (new_format != LINEAR_UINT8) {
            if (new_channels != input.channels()) {
                throw std::invalid_argument("Cannot change channel count of compressed images");
            }
        }

        fmt = input.format();
        w = input.width();
        h = input.height();
        cc = new_channels;

        d = new uint8_t[bsz];
        if (new_channels == input.channels()) {
            memcpy(d, input.data(), bsz);
        } else {
            assert(input.format() == LINEAR_UINT8);

            const uint8_t *inp = static_cast<const uint8_t *>(input.data());
            uint8_t *outp = static_cast<uint8_t *>(d);

            for (int i = 0; i < w * h; i++) {
                for (int c = 0; c < cc; c++) {
                    *(outp++) = (c < input.channels() ? *(inp++) : 0);
                }
                for (int c = 0; c < input.channels() - cc; c++) {
                    inp++;
                }
            }
        }
    } else if (new_format == LINEAR_UINT8) {
        throw std::invalid_argument("Decompression is not yet supported");
    } else if (input.format() == LINEAR_UINT8) {
        fmt = new_format;
        w = input.width();
        h = input.height();
        cc = (new_format == COMPRESSED_S3TC_DXT1) ? 3 : 4;

        d = new uint8_t[bsz];
        tx_compress_dxtn(input.channels(), w, h, static_cast<const uint8_t *>(input.data()), gl_types[fmt],
                         static_cast<uint8_t *>(d), 0);
    } else {
        throw std::invalid_argument("Recompression is not supported");
    }
}


dake::gl::image::~image(void)
{
    // FIXME (should use the correct type)
    delete[] static_cast<uint8_t *>(d);
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

            assert(fmt == LINEAR_UINT8);
            bsz = w * h * cc;

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
    if (compressed()) {
        return gl_type();
    } else {
        return gl_formats[cc - 1];
    }
}


GLenum dake::gl::image::gl_type(void) const
{
    return gl_types[fmt];
}


bool dake::gl::image::compressed(void) const
{
    switch (fmt) {
        case LINEAR_UINT8:
            return false;

        case COMPRESSED_S3TC_DXT1:
        case COMPRESSED_S3TC_DXT1_ALPHA:
        case COMPRESSED_S3TC_DXT3:
        case COMPRESSED_S3TC_DXT5:
            return true;

        default:
            abort();
    }
}
