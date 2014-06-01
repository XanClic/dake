#ifndef DAKE__GL__SHADER_HPP
#define DAKE__GL__SHADER_HPP

#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "dake/gl/gl.hpp"


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


class program;

extern program *active_program;


template<typename T> class uniform;

class program
{
    private:
        GLuint id;
        std::unordered_map<std::string, GLint> uniform_locations;


    public:
        program(void);
        ~program(void);

        void operator<<(const shader &sh);

        bool link(void);
        void use(void);

        static void unuse(void) { glUseProgram(0); active_program = nullptr; }

        GLuint attrib(const char *identifier);
        void bind_attrib(const char *identifier, int location);
        void bind_frag(const char *identifier, int location);

        template<typename T> dake::gl::uniform<T> uniform(const std::string &identifier)
        {
            auto pos = uniform_locations.find(identifier);
            if (pos != uniform_locations.end()) {
                return dake::gl::uniform<T>(pos->second, this);
            }

            GLint uni_id = glGetUniformLocation(id, identifier.c_str());
            if (uni_id < 0) {
                throw std::invalid_argument(std::string("Could not find uniform ") + identifier);
            }
            uniform_locations.emplace(identifier, uni_id);
            return dake::gl::uniform<T>(uni_id, this);
        }
};


template<typename T> class uniform
{
    private:
        GLint id;
        program *prg;

        void assign(const T &value);


    public:
        uniform(void) { id = -1; prg = nullptr; }
        uniform(GLint id, program *prg)
        {
            assert(id >= 0);
            this->id = id;
            this->prg = prg;
        }

        uniform<T> &operator=(const T &value)
        {
            if ((id < 0) || !prg) throw std::invalid_argument("Uniform has not been looked up yet");
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
