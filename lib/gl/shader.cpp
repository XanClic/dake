#include <cstdio>
#include <initializer_list>
#include <stdexcept>
#include <string>

#include <dake/math/matrix.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/texture.hpp>


namespace dake
{

namespace gl
{

program *active_program;

}

}


dake::gl::shader::shader(dake::gl::shader::type tp):
    t(tp)
{
    id = glCreateShader(tp);

    if (!id) {
        throw std::runtime_error("Could not create shader");
    }
}


dake::gl::shader::shader(dake::gl::shader::type tp, const char *src_file):
    shader(tp)
{
    load(src_file);
}


dake::gl::shader::~shader(void)
{
    glDeleteShader(id);
}


void dake::gl::shader::load(const char *file)
{
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        throw std::invalid_argument("Could not open the given shader file");
    }

    name = std::string(file);

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


void dake::gl::shader::source(const char *src)
{
    name = std::string("unknown");

    glShaderSource(id, 1, const_cast<const GLchar **>(&src), nullptr);
}


static const char *shader_type_string(GLint t)
{
    switch (t) {
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
    if (status == GL_TRUE) {
        compiled = true;
        return true;
    }

    GLint illen;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &illen);

    if (illen <= 1) {
        throw std::string("Error compiling " + std::string(shader_type_string(t)) + " shader " + name + ": Reason unknown");
    } else {
        char *msg = new char[illen + 1];

        glGetShaderInfoLog(id, illen, nullptr, msg);
        msg[illen] = 0;

        std::string msg_str(msg);
        delete[] msg;

        throw std::string("Error compiling " + std::string(shader_type_string(t)) + " shader " + name + ": " + msg_str);
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


void dake::gl::program::operator<<(dake::gl::shader &sh)
{
    if (!sh.compiled) {
        sh.compile();
    }

    if (name.empty()) {
        name = sh.name;
    } else {
        name += ", " + sh.name;
    }

    glAttachShader(id, sh.id);
}


bool dake::gl::program::link(void)
{
    glLinkProgram(id);

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_TRUE) {
        linked = true;
        return true;
    }

    GLint illen;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &illen);

    if (illen <= 1) {
        throw std::runtime_error("Error linking shaders " + name + " to program: Reason unknown");
    } else {
        char *msg = new char[illen + 1];

        glGetProgramInfoLog(id, illen, nullptr, msg);
        msg[illen] = 0;

        std::string msg_str(msg);
        delete[] msg;

        throw std::runtime_error("Error linking shaders " + name + " to program: " + msg_str);
    }
}


void dake::gl::program::use(void)
{
    if (dake::gl::active_program == this) {
        return;
    }

    if (!linked) {
        link();
    }

    glUseProgram(id);
    dake::gl::active_program = this;
}


GLuint dake::gl::program::attrib(const char *identifier)
{
    if (!linked) {
        link();
    }
    return glGetAttribLocation(id, identifier);
}


void dake::gl::program::bind_attrib(const char *identifier, int location)
{
    glBindAttribLocation(id, location, identifier);
}


GLuint dake::gl::program::frag(const char *identifier)
{
    if (!linked) {
        link();
    }
    return glGetFragDataLocation(id, identifier);
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
