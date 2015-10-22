#include "dake/gl/gl.hpp"

#include <cstring>


namespace dake
{

namespace gl
{

glext_info glext;

const char *extension_names[] = {
    "GL_ARB_bindless_texture",
    "GL_ARB_stencil_texturing",
    "GL_ARB_texture_view",
};

}

}


bool dake::gl::glext_info::init(void)
{
    if (initialized) {
        return true;
    }

    GLint ext_count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);

    for (int i = 0; i < ext_count; i++) {
        const GLubyte *ext = glGetStringi(GL_EXTENSIONS, i);
        if (ext) {
            exts.push_back(reinterpret_cast<const char *>(ext));
        }
    }


    exts_map.resize(sizeof(extension_names) / sizeof(extension_names[0]));
    for (int i = 0; i < static_cast<int>(exts_map.size()); i++) {
        exts_map[i] = has_extension(extension_names[i]);
    }


    initialized = true;

    return true;
}


bool dake::gl::glext_info::has_extension(const char *name) const
{
    for (const std::string &ext: exts) {
        if (!strcmp(ext.c_str(), name)) {
            return true;
        }
    }

    return false;
}


bool dake::gl::glext_info::has_extension(const std::string &name) const
{
    return has_extension(name.c_str());
}
