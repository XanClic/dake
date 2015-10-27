#include <cstdio>
#include <initializer_list>
#include <stdexcept>
#include <string>

#include <dake/math/fmatrix.hpp>
#include <dake/math/matrix.hpp>
#include <dake/gl/find_resource.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/texture.hpp>


namespace dake
{

namespace gl
{

program *active_program;

}

}


dake::gl::shader::shader(GLint tp, GLuint glid):
    id(glid), t(tp), is_copy(true)
{}


dake::gl::shader::shader(dake::gl::shader::type tp, const char *src_file):
    t(tp), name(src_file)
{
    id = glCreateShader(tp);
    if (!id) {
        throw std::runtime_error("Could not create shader");
    }

    if (src_file) {
        load(src_file);
    }
}


dake::gl::shader::shader(dake::gl::shader &&sh)
{
    id = sh.id;
    t = sh.t;
    compiled = sh.compiled;
    name = std::move(sh.name);

    sh.t = 0;
    sh.id = 0;
    sh.compiled = false;
    sh.name = "unnamed";
}


dake::gl::shader::~shader(void)
{
    if (id && !is_copy) {
        glDeleteShader(id);
    }
}


dake::gl::shader dake::gl::shader::vert(const char *src_file)
{
    return shader(VERTEX, src_file);
}


dake::gl::shader dake::gl::shader::geom(const char *src_file)
{
    return shader(GEOMETRY, src_file);
}


dake::gl::shader dake::gl::shader::frag(const char *src_file)
{
    return shader(FRAGMENT, src_file);
}


void dake::gl::shader::check_valid(void) const
{
    if (!id) {
        throw std::runtime_error("Shader " + name + " is not valid; may have been move'd");
    }
}


void dake::gl::shader::load(const char *file)
{
    check_valid();

    FILE *fp = fopen(dake::gl::find_resource_filename(file).c_str(), "rb");
    if (!fp) {
        throw std::invalid_argument("Could not open the given shader file");
    }

    name = std::string(file);

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);

    char *src = new char[len + 1];
    if (fread(src, 1, len, fp) < len) {
        const char *err = strerror(errno);
        fclose(fp);
        delete[] src;
        throw std::runtime_error("Failed to read shader code from " + name
                                 + ": " + err);
    }
    src[len] = 0;

    fclose(fp);

    glShaderSource(id, 1, const_cast<const GLchar **>(&src), nullptr);
    delete[] src;
}


void dake::gl::shader::source(const char *src)
{
    check_valid();

    name = "unknown";

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
    check_valid();

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
        throw std::runtime_error("Error compiling " + std::string(shader_type_string(t)) + " shader " + name + ": Reason unknown");
    } else {
        char *msg = new char[illen + 1];

        glGetShaderInfoLog(id, illen, nullptr, msg);
        msg[illen] = 0;

        std::string msg_str(msg);
        delete[] msg;

        throw std::runtime_error("Error compiling " + std::string(shader_type_string(t)) + " shader " + name + ": " + msg_str);
    }

    return false;
}


dake::gl::program::program(void)
{
    id = glCreateProgram();
}


dake::gl::program::program(std::initializer_list<shader> shaders):
    program()
{
    for (const shader &sh: shaders) {
        // Let's just pray to god this works
        shader sh_copy(sh.t, sh.id);
        *this << sh_copy;
    }
}


dake::gl::program::program(program &&prg)
{
    id = prg.id;
    uniform_locations = std::move(prg.uniform_locations);
    name = std::move(prg.name);

    prg.id = 0;
    prg.name = "";
}


dake::gl::program::~program(void)
{
    if (id) {
        glDeleteProgram(id);
    }
}


void dake::gl::program::check_valid(void) const
{
    if (!id) {
        throw std::runtime_error("Program (from shaders " + name + ") is not valid; may have been move'd");
    }
}


void dake::gl::program::operator<<(dake::gl::shader &sh)
{
    check_valid();

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


void dake::gl::program::operator<<(dake::gl::shader &&sh)
{
    check_valid();

    shader sh_moved(sh.t, sh.id);

    sh.id = 0;
    sh.compiled = false;

    *this << sh_moved;
}


bool dake::gl::program::link(void)
{
    check_valid();

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
    check_valid();

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
    check_valid();

    if (!linked) {
        link();
    }
    return glGetAttribLocation(id, identifier);
}


void dake::gl::program::bind_attrib(const char *identifier, int location)
{
    check_valid();
    glBindAttribLocation(id, location, identifier);
}


GLuint dake::gl::program::frag(const char *identifier)
{
    check_valid();

    if (!linked) {
        link();
    }
    return glGetFragDataLocation(id, identifier);
}


void dake::gl::program::bind_frag(const char *identifier, int location)
{
    check_valid();
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
template<> void uniform<math::fmat3>::assign(const math::fmat3 &value)
{ glUniformMatrix3fv(id, 1, false, (math::mat3)value); }
template<> void uniform<math::fmat4>::assign(const math::fmat4 &value)
{ glUniformMatrix4fv(id, 1, false, value.d); }
template<> void uniform<math::vec2>::assign(const math::vec2 &value)
{ glUniform2fv(id, 1, value); }
template<> void uniform<math::vec3>::assign(const math::vec3 &value)
{ glUniform3fv(id, 1, value); }
template<> void uniform<math::vec4>::assign(const math::vec4 &value)
{ glUniform4fv(id, 1, value); }
template<> void uniform<math::fvec2>::assign(const math::fvec2 &value)
{ glUniform2fv(id, 1, value); }
template<> void uniform<math::fvec3>::assign(const math::fvec3 &value)
{ glUniform3fv(id, 1, value); }
template<> void uniform<math::fvec4>::assign(const math::fvec4 &value)
{ glUniform4fv(id, 1, value); }
template<> void uniform<float>::assign(const float &value)
{ glUniform1f(id, value); }
template<> void uniform<uint32_t>::assign(const uint32_t &value)
{ glUniform1ui(id, value); }
template<> void uniform<int32_t>::assign(const int32_t &value)
{ glUniform1i(id, value); }

template<> void uniform<texture>::assign(const texture &value)
{
    if (value.bindless()) {
        glUniformHandleui64ARB(id, value.handle());
    } else {
        glUniform1i(id, value.tmu());
    }
}

template<> void uniform<array_texture>::assign(const array_texture &value)
{
    if (value.bindless()) {
        glUniformHandleui64ARB(id, value.handle());
    } else {
        glUniform1i(id, value.tmu());
    }
}

template<> void uniform<cubemap>::assign(const cubemap &value)
{
    if (value.bindless()) {
        glUniformHandleui64ARB(id, value.handle());
    } else {
        glUniform1i(id, value.tmu());
    }
}

}

}
