#ifndef DAKE__GL__GL_HPP
#define DAKE__GL__GL_HPP

#include <epoxy/gl.h>
#include <epoxy/glx.h>


#include <string>
#include <vector>


namespace dake
{

namespace gl
{

enum extension {
    BINDLESS_TEXTURE,
    STENCIL_TEXTURING,
    TEXTURE_VIEW,
};

extern const char *extension_names[];

class glext_info {
    private:
        bool initialized = false;
        std::vector<std::string> exts;
        std::vector<bool> exts_map;


    public:
        bool init(void);

        const std::vector<std::string> &extensions(void) const
        { return exts; }

        bool has_extension(const char *name) const;
        bool has_extension(const std::string &name) const;
        bool has_extension(extension ext) const
        { return exts_map[static_cast<int>(ext)]; }

        bool has_bindless_textures(void) const
        { return has_extension(BINDLESS_TEXTURE); }
};

extern glext_info glext;

static inline bool glext_init(void) { return glext.init(); }

}

}

#endif
