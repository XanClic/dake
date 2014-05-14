#ifndef DAKE__GL__SHADER_HPP
#define DAKE__GL__SHADER_HPP

#include <stdexcept>
#include <string>
#include <QtOpenGL>


namespace dake
{

namespace gl
{

class shader
{
    private:
        GLuint id;
        GLint t;

        friend class program;


    public:
        enum type
        {
            VERTEX   = GL_VERTEX_SHADER,
            FRAGMENT = GL_FRAGMENT_SHADER,
            GEOMETRY = GL_GEOMETRY_SHADER
        };

        shader(type t);
        ~shader(void);

        void load(const char *file);
        void source(const char *source);

        bool compile(void);
};


template<typename T> class uniform;

class program
{
    private:
        GLuint id;


    public:
        program(void);
        ~program(void);

        void operator<<(const shader &sh);

        bool link(void);
        void use(void);

        static void unuse(void) { glUseProgram(0); }

        GLuint attrib(const char *identifier);
        void bind_attrib(const char *identifier, int location);
        void bind_frag(const char *identifier, int location);

        template<typename T> dake::gl::uniform<T> uniform(const char *identifier)
        { return dake::gl::uniform<T>(glGetUniformLocation(id, identifier), this, identifier); }
};


template<typename T> class uniform
{
    private:
        GLint id;
        program *prg;

        void assign(const T &value);


    public:
        uniform(void) { id = -1; prg = NULL; }
        uniform(GLint id, program *prg, const char *name = nullptr)
        {
            if (id < 0) throw std::runtime_error("Could not find uniform " + std::string(name ? name : "(unknown)"));
            this->id = id;
            this->prg = prg;
        }

        uniform<T> &operator=(const T &value)
        {
            if ((id < 0) || !prg) throw std::runtime_error("Uniform has not been looked up yet");
            prg->use();
            assign(value);
            return *this;
        }

        uniform<T> &operator=(const uniform<T> &from)
        { id = from.id; prg = from.prg; return *this; }
};

}

}

#endif
