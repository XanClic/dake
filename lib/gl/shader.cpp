#include <cstdio>
#include <stdexcept>

#include <dake/math/matrix.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/texture.hpp>


dake::gl::shader::shader(dake::gl::shader::type tp):
    t(tp)
{
    id = glCreateShader(tp);

    if (!id)
        throw std::runtime_error("Could not create shader");
}


dake::gl::shader::~shader(void)
{
    glDeleteShader(id);
}


void dake::gl::shader::load(const char *file)
{
    FILE *fp = fopen(file, "rb");
    if (!fp)
        throw std::invalid_argument("Could not open the given shader file");

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);

    char *src = new char[len + 1];
    fread(src, 1, len, fp);
    src[len] = 0;

    fclose(fp);

    glShaderSource(id, 1, const_cast<const GLchar **>(&src), nullptr);
    delete src;
}


void dake::gl::shader::source(const char *source)
{
    glShaderSource(id, 1, const_cast<const GLchar **>(&source), nullptr);
}


static const char *shader_type_string(GLint t)
{
    switch (t)
    {
        case GL_VERTEX_SHADER:
            return "vertex";
        case GL_FRAGMENT_SHADER:
            return "fragment";
        case GL_GEOMETRY_SHADER:
            return "geometry";
        default:
            return "unknown";
    }
}


bool dake::gl::shader::compile(void)
{
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
        return true;

    GLint illen;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &illen);

    if (illen <= 1)
        fprintf(stderr, "Error compiling %s shader: Reason unknown\n", shader_type_string(t));
    else
    {
        char *msg = new char[illen + 1];

        glGetShaderInfoLog(id, illen, NULL, msg);
        msg[illen] = 0;

        fprintf(stderr, "Error compiling %s shader: %s\n", shader_type_string(t), msg);

        delete msg;
    }

    return false;
}


dake::gl::program::program(void)
{
    id = glCreateProgram();
}


dake::gl::program::~program(void)
{
    glDeleteProgram(id);
}


void dake::gl::program::operator<<(const dake::gl::shader &sh)
{
    glAttachShader(id, sh.id);
}


bool dake::gl::program::link(void)
{
    glLinkProgram(id);

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_TRUE)
        return true;

    GLint illen;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &illen);

    if (illen <= 1)
        fprintf(stderr, "Error linking program: Reason unknown\n");
    else
    {
        char *msg = new char[illen + 1];

        glGetProgramInfoLog(id, illen, NULL, msg);
        msg[illen] = 0;

        fprintf(stderr, "Error linking program: %s\n", msg);

        delete msg;
    }

    return false;
}


void dake::gl::program::use(void)
{
    glUseProgram(id);
}


GLuint dake::gl::program::attrib(const char *identifier)
{
    return glGetAttribLocation(id, identifier);
}


void dake::gl::program::bind_attrib(const char *identifier, int location)
{
    glBindAttribLocation(id, location, identifier);
}


void dake::gl::program::bind_frag(const char *identifier, int location)
{
    glBindFragDataLocation(id, location, identifier);
}


namespace dake
{

namespace gl
{

template<> void uniform<math::mat2>::assign(const math::mat2 &value)
{ glUniformMatrix2fv(id, 1, false, value); }
template<> void uniform<math::mat3>::assign(const math::mat3 &value)
{ glUniformMatrix3fv(id, 1, false, value); }
template<> void uniform<math::mat4>::assign(const math::mat4 &value)
{ glUniformMatrix4fv(id, 1, false, value); }
template<> void uniform<math::vec2>::assign(const math::vec2 &value)
{ glUniform2fv(id, 1, value); }
template<> void uniform<math::vec3>::assign(const math::vec3 &value)
{ glUniform3fv(id, 1, value); }
template<> void uniform<math::vec4>::assign(const math::vec4 &value)
{ glUniform4fv(id, 1, value); }
template<> void uniform<float>::assign(const float &value)
{ glUniform1f(id, value); }
template<> void uniform<texture>::assign(const texture &value)
{ glUniform1i(id, value.get_tmu()); }

}

}
