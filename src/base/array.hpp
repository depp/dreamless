/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_BASE_ARRAY_HPP
#define LD_BASE_ARRAY_HPP
#include "sg/opengl.h"
#include "sg/entry.h"
#include <limits>
#include <new>
#include <cstdlib>
namespace Base {

// OpenGL attribute array data types.
template<typename T>
struct ArrayType { };

template<>
struct ArrayType<short> {
    static const GLenum TYPE = GL_SHORT;
    static const int SIZE = 1;
};

template<>
struct ArrayType<short[2]> {
    static const GLenum TYPE = GL_SHORT;
    static const int SIZE = 2;
};

template<>
struct ArrayType<short[3]> {
    static const GLenum TYPE = GL_SHORT;
    static const int SIZE = 3;
};

template<>
struct ArrayType<short[4]> {
    static const GLenum TYPE = GL_SHORT;
    static const int SIZE = 4;
};

template<>
struct ArrayType<float> {
    static const GLenum TYPE = GL_FLOAT;
    static const int SIZE = 1;
};

template<>
struct ArrayType<float[2]> {
    static const GLenum TYPE = GL_FLOAT;
    static const int SIZE = 2;
};

template<>
struct ArrayType<float[3]> {
    static const GLenum TYPE = GL_FLOAT;
    static const int SIZE = 3;
};

template<>
struct ArrayType<float[4]> {
    static const GLenum TYPE = GL_FLOAT;
    static const int SIZE = 4;
};

// OpenGL attribute array class.
template<class T>
class Array {
private:
    T *m_data;
    int m_count;
    int m_alloc;
    bool m_dirty;
    GLuint m_buffer;

public:
    explicit Array();
    Array(const Array &other) = delete;
    Array(Array &&other);
    ~Array();
    Array &operator=(const Array &other) = delete;
    Array &operator=(Array &&other);

    /// Get the number of elements in the array.
    int size() const;
    /// Determine whether the array is empty.
    bool empty() const;
    /// Set the number of elements in the array to zero.
    void clear();
    /// Reserve space for the given total number of elements.
    void reserve(std::size_t total);
    /// Insert the given number of elements and return a pointer to the first.
    T *insert(std::size_t count);
    /// Upload the array to an OpenGL buffer.
    void upload(GLenum usage);
    /// Set the array as a vertex attribute.
    void set_attrib(GLint attrib);
};

template<class T>
inline Array<T>::Array()
    : m_data(nullptr), m_count(0), m_alloc(0), m_dirty(true), m_buffer(0)
{ }

template<class T>
inline Array<T>::Array(Array<T> &&other)
    : m_data(nullptr), m_count(0), m_alloc(0), m_dirty(true), m_buffer(0) {
    m_data = other.m_data;
    m_count = other.m_count;
    m_alloc = other.m_alloc;
    m_dirty = other.m_dirty;
    m_buffer = other.m_buffer;
    other.m_data = nullptr;
    other.m_count = 0;
    other.m_alloc = 0;
    other.m_dirty = true;
    other.m_buffer = 0;
}

template<class T>
inline Array<T>::~Array() {
    std::free(m_data);
    glDeleteBuffers(1, &m_buffer);
}

template<class T>
Array<T> &Array<T>::operator=(Array<T> &&other) {
    if (this == &other)
        return *this;
    std::free(m_data);
    m_data = other.m_data;
    m_count = other.m_count;
    m_alloc = other.m_alloc;
    m_dirty=  other.m_dirty;
    m_buffer = other.m_buffer;
    other.m_data = nullptr;
    other.m_count = 0;
    other.m_alloc = 0;
    other.m_dirty = true;
    other.m_buffer = 0;
    return *this;
}

template<class T>
int Array<T>::size() const {
    return m_count;
}

template<class T>
bool Array<T>::empty() const {
    return m_count == 0;
}

template<class T>
void Array<T>::clear() {
    m_count = 0;
}

template<class T>
void Array<T>::reserve(std::size_t total) {
    std::size_t rounded;
    if (total > std::numeric_limits<int>::max())
        sg_sys_abort("out of memory");
    if (m_alloc >= total)
        return;
    rounded = total - 1;
    rounded |= rounded >> 1;
    rounded |= rounded >> 2;
    rounded |= rounded >> 4;
    rounded |= rounded >> 8;
    rounded |= rounded >> 16;
    rounded += 1;
    T *newdata = static_cast<T *>(std::realloc(m_data, sizeof(T) * rounded));
    if (!newdata)
        sg_sys_abort("out of memory");
    m_data = newdata;
    m_alloc = rounded;
}

template<class T>
T *Array<T>::insert(std::size_t count) {
    if (count > m_alloc - m_count) {
        if (count > std::numeric_limits<int>::max() - m_count)
            sg_sys_abort("out of memory");
        reserve(count + m_count);
    }
    int pos = m_count;
    m_count += count;
    m_dirty = true;
    return m_data + pos;
}

template<class T>
void Array<T>::upload(GLenum usage) {
    if (!m_dirty)
        return;
    if (m_buffer == 0)
        glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_count * sizeof(T), m_data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<class T>
void Array<T>::set_attrib(GLint attrib) {
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glVertexAttribPointer(
        attrib, ArrayType<T>::SIZE, ArrayType<T>::TYPE,
        GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}
#endif
