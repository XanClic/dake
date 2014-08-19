#include "dake/gl/gl.hpp"

#include <cstring>


namespace dake
{

namespace gl
{

glext_info glext;

}

}


bool dake::gl::glext_info::init(void)
{
    if (initialized) {
        return true;
    }

#ifdef __MINGW32__
    if (glewInit() != GLEW_OK) {
        return false;
    }
#endif

    GLint ext_count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);

    for (int i = 0; i < ext_count; i++) {
        exts.push_back(reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i)));
    }


    bindless_txt = has_extension("GL_ARB_bindless_texture");


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
