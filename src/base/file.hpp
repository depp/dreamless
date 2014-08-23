/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_BASE_FILE_HPP
#define LD_BASE_FILE_HPP
#include "sg/file.h"
#include <cstddef>
#include <string>
namespace Base {

class Data {
    sg_buffer *m_buffer;

public:
    Data() : m_buffer(nullptr) { }
    Data(const Data &other) : m_buffer(other.m_buffer) {
        if (m_buffer)
            sg_buffer_incref(m_buffer);
    }
    Data(Data &&other) : m_buffer(other.m_buffer) {
        other.m_buffer = nullptr;
    }
    ~Data() {
        if (m_buffer)
            sg_buffer_decref(m_buffer);
    }
    Data &operator=(const Data &other) {
        if (other.m_buffer)
            sg_buffer_incref(other.m_buffer);
        if (m_buffer)
            sg_buffer_decref(m_buffer);
        m_buffer = other.m_buffer;
        return *this;
    }
    Data &operator=(Data &&other) {
        sg_buffer *tmp = other.m_buffer;
        other.m_buffer = nullptr;
        m_buffer = tmp;
        return *this;
    }

    /// Get the start of the buffer.
    const void *ptr() const { return m_buffer ? m_buffer->data : nullptr; }
    /// Get the number of bytes in the buffer.
    std::size_t size() const { return m_buffer ? m_buffer->length : 0; }
    /// Read the contents of a file.
    void read(const std::string &path, size_t maxsz) {
        read(path, maxsz, nullptr);
    }
    /// Read the contents of a file.
    void read(const std::string &path, size_t maxsz,
              const char *extensions);
};

}
#endif
