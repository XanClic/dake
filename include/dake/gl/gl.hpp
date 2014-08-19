#ifndef DAKE__GL__GL_HPP
#define DAKE__GL__GL_HPP

#ifdef __MINGW32__
extern "C" {
#include <GL/glew.h>
}
#else
#define GL_GLEXT_PROTOTYPES
#endif

extern "C" {
#include <GL/gl.h>
}

#ifndef __MINGW32__
extern "C" {
#include <GL/glext.h>
}
#endif


#include <string>
#include <vector>


namespace dake
{

namespace gl
{

class glext_info {
    private:
        bool initialized = false;
        std::vector<std::string> exts;
        bool bindless_txt = false;


    public:
        bool init(void);

        const std::vector<std::string> &extensions(void) const
        { return exts; }

        bool has_extension(const char *name) const;
        bool has_extension(const std::string &name) const;

        bool has_bindless_textures(void) const
        { return bindless_txt; }
};

extern glext_info glext;

static inline bool glext_init(void) { return glext.init(); }

}

}

#endif
