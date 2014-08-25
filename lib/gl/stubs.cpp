#include "dake/gl/gl.hpp"

#include <stdexcept>


#ifdef GL_GLEXT_PROTOTYPES

extern "C" {
GLuint64 glGetTextureHandleARB(GLuint texture) __attribute__((weak));
void glMakeTextureHandleResidentARB(GLuint64 handle) __attribute__((weak));
void glMakeTextureHandleNonResidentARB(GLuint64 handle) __attribute__((weak));
void glUniformHandleui64ARB(GLint location, GLuint64 value) __attribute__((weak));
};

GLuint64 glGetTextureHandleARB(GLuint texture)
{
    (void)texture;
    throw std::runtime_error("glGetTextureHandleARB() not available");
}

void glMakeTextureHandleResidentARB(GLuint64 handle)
{
    (void)handle;
    throw std::runtime_error("glMakeTextureHandleResidentARB() not available");
}

void glMakeTextureHandleNonResidentARB(GLuint64 handle)
{
    (void)handle;
    throw std::runtime_error("glMakeTextureHandleNonResidentARB() not available");
}

void glUniformHandleui64ARB(GLint location, GLuint64 value)
{
    (void)location;
    (void)value;
    throw std::runtime_error("glUniformHandleui64ARB() not available");
}

#endif
