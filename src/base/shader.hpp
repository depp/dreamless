/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_BASE_SHADER_HPP
#define LD_BASE_SHADER_HPP
#include "sg/opengl.h"
#include <cstddef>
#include <string>
namespace Base {

/// A field in an object which stores program attributes and uniform indexes.
struct ShaderField {
    /// The name of the uniform or attribute.
    const char *name;

    /// The offset of the corresponding GLint field in the structure.
    std::size_t offset;
};

/// Load an OpenGL shader program.  Returns 0 on failure.
GLuint load_program(const std::string &vertexshader,
                    const std::string &fragmentshader,
                    const ShaderField *uniforms,
                    const ShaderField *attributes,
                    void *object);

/// An OpenGL shader program.  The parameter T has uniforms and attributes.
template<class T>
class Program {
private:
    GLuint prog_;
    T fields_;

public:
    Program(const std::string &vertexshader,
            const std::string &fragmentshader);
    Program(const Program &) = delete;
    ~Program();
    Program &operator =(const Program &) = delete;

    /// Get program attribute and uniform indexes.
    const T *operator->() const { return &fields_; }
    /// Get the program object.
    GLuint prog() const { return prog_; }
};

template<class T>
Program<T>::Program(const std::string &vertexshader,
                    const std::string &fragmentshader)
    : prog_(0), fields_() {
    prog_ = load_program(
        vertexshader, fragmentshader,
        T::UNIFORMS, T::ATTRIBUTES,
        &fields_);
}

template<class T>
Program<T>::~Program() {
    glDeleteProgram(prog_);
}

}
#endif
