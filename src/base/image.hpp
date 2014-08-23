/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_BASE_IMAGE_HPP
#define LD_BASE_IMAGE_HPP
#include "sg/opengl.h"
#include "sg/pixbuf.h"
#include <string>
namespace Base {

class Image {
    sg_pixbuf m_pixbuf;

public:
    Image() {
        sg_pixbuf_init(&m_pixbuf);
    }

    Image(const Image &) = delete;

    Image(Image &&other) {
        m_pixbuf = other.m_pixbuf;
        sg_pixbuf_init(&other.m_pixbuf);
    }

    ~Image() {
        sg_pixbuf_destroy(&m_pixbuf);
    }

    Image &operator=(const Image &) = delete;

    Image &operator=(Image &&other) {
        sg_pixbuf_destroy(&m_pixbuf);
        m_pixbuf = other.m_pixbuf;
        sg_pixbuf_init(&other.m_pixbuf);
        return *this;
    }

    const sg_pixbuf *operator->() const { return &m_pixbuf; }

    void set(sg_pixbuf_format_t format, int width, int height);
    void alloc();
    void calloc();
    void load(const std::string &path);
    void copy_from(const Image &other, int x, int y);
};

struct Texture {
    GLuint tex;
    short iwidth;
    short iheight;
    short twidth;
    short theight;
    float scale[2];

    Texture();
    Texture(const Texture &other) = delete;
    Texture(Texture &&);
    ~Texture();
    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&other);

    /// Load an image as a 2-dimensional texture.
    static Texture load(const std::string &path);

    /// Load an image as a 2-dimensional texture.
    static Texture load(const Image &image);

    /// Load an image as a 1-dimensional texture.
    static Texture load_1d(const std::string &path);

    /// Load an image as a 2-dimensional texture.
    static Texture load_1d(const Image &image);
};

}
#endif
