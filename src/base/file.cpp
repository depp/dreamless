/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sg/entry.h"
#include "file.hpp"
#include <cstdlib>
namespace Base {

void Data::read(const std::string &path, size_t maxsz,
                const char *extensions) {
    sg_buffer *nbuf;
    nbuf = sg_file_get(path.data(), path.size(), SG_RDONLY,
                       extensions, maxsz, nullptr);
    if (!nbuf)
        sg_sys_abortf("could not read file: %s", path.c_str());
    if (m_buffer)
        sg_buffer_decref(m_buffer);
    m_buffer = nbuf;
}

}
