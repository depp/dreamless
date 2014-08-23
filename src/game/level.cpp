/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "level.hpp"
#include "base/file.hpp"
#include <cstring>
#include <cstdio>
#include <vector>
namespace Game {

Level::Level()
    : m_width(0), m_height(0), m_data(nullptr)
{ }

Level::Level(Level &&other)
    : m_width(other.m_width),
      m_height(other.m_height),
      m_data(other.m_data) {
    other.m_width = 0;
    other.m_height = 0;
    other.m_data = nullptr;
}

Level::~Level() {
    if (m_data)
        delete[] m_data;
}

Level &Level::operator=(Level &&other) {
    int width = other.m_width, height = other.m_height;
    unsigned char *data = other.m_data;
    other.m_width = 0;
    other.m_height = 0;
    other.m_data = nullptr;
    if (m_data)
        delete[] m_data;
    m_width = width;
    m_height = height;
    m_data = data;
    return *this;
}

void Level::load(const std::string &name) {
    static size_t MAX_SIZE = 1024 * 32;
    std::string path("level/");
    path += name;
    path += ".txt";
    Base::Data data;
    data.read(path, MAX_SIZE);

    typedef std::pair<const char *, std::size_t> Line;
    std::vector<Line> lines;
    {
        const char *ptr = static_cast<const char *>(data.ptr());
        const char *end = ptr + data.size();
        while (ptr != end) {
            const char *eol = static_cast<const char *>(
                memchr(ptr, '\n', end - ptr));
            if (!eol) {
                lines.push_back(Line(ptr, end - ptr));
                break;
            }
            lines.push_back(Line(ptr, eol - ptr));
            ptr = eol + 1;
        }
    }

    int rows = lines.size();
    int columns = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].second > 0 && lines[i].first[0] == '-') {
            rows = i;
            break;
        }
        if (lines[i].second > columns)
            columns = lines[i].second;
    }

    std::printf("Size: %d x %d\n", columns, rows);
}

}
