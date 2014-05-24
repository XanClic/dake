#ifndef DAKE__GL__GL_HPP
#define DAKE__GL__GL_HPP

#ifdef __MINGW32__
extern "C" {
#include <GL/glew.h>
}

namespace dake { namespace gl { static inline bool glext_init(void) { return glewInit() == GLEW_OK; } } }
#else
#define GL_GLEXT_PROTOTYPES
namespace dake { namespace gl { static inline bool glext_init(void) { return true; } } }
#endif

extern "C" {
#include <GL/gl.h>
}

#ifndef __MINGW32__
extern "C" {
#include <GL/glext.h>
}
#endif

#endif
