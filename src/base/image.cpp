/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sg/entry.h"
#include "file.hpp"
#include "image.hpp"
#include <cstdlib>
#include <cstring>
namespace Base {

void Image::set(sg_pixbuf_format_t format, int width, int height) {
    int r = sg_pixbuf_set(&m_pixbuf, format, width, height, nullptr);
    if (r)
        sg_sys_abort("Image::set");
}

void Image::alloc() {
    int r = sg_pixbuf_alloc(&m_pixbuf, nullptr);
    if (r)
        sg_sys_abort("Image::alloc");
}

void Image::calloc() {
    int r = sg_pixbuf_calloc(&m_pixbuf, nullptr);
    if (r)
        sg_sys_abort("Image::calloc");
}

void Image::load(const std::string &path) {
    static const int MAX_SIZE = 1024 * 1024 * 16;
    Data data;
    data.read(path, MAX_SIZE, SG_PIXBUF_IMAGE_EXTENSIONS);
    int r = sg_pixbuf_loadimage(
        &m_pixbuf, data.ptr(), data.size(), nullptr);
    if (r)
        sg_sys_abortf("could not load image: %s", path.c_str());
}

void Image::copy_from(const Image &other, int x, int y) {
    if (x < 0 || y < 0 ||
        other->iwidth > m_pixbuf.iwidth ||
        other->iheight > m_pixbuf.iheight ||
        x > m_pixbuf.iwidth - other->iwidth ||
        y > m_pixbuf.iheight - other->iheight)
        sg_sys_abort("invalid image copy");

    const unsigned char *ip = static_cast<const unsigned char *>(other->data);
    unsigned char *op = static_cast<unsigned char *>(m_pixbuf.data);
    size_t irb = other->rowbytes, orb = m_pixbuf.rowbytes;
    size_t xn = other->iwidth, yn = other->iheight;

    if (other->format == m_pixbuf.format) {
        size_t psz = SG_PIXBUF_FORMATSIZE[m_pixbuf.format];
        for (size_t yi = 0; yi < yn; yi++)
            std::memcpy(op + orb * (yi + y) + psz * x,
                        ip + irb * yi, psz * xn);
    } else if (m_pixbuf.format == SG_RGBA && other->format == SG_RGB) {
        for (size_t yi = 0; yi < yn; yi++) {
            const unsigned char *irp = ip + irb * yi;
            unsigned char *orp = op + orb * (yi + y);
            for (size_t xi = 0; xi < xn; xi++) {
                orp[4 * (x + xi) + 0] = irp[3 * xi + 0];
                orp[4 * (x + xi) + 1] = irp[3 * xi + 1];
                orp[4 * (x + xi) + 2] = irp[3 * xi + 2];
                orp[4 * (x + xi) + 3] = 255;
            }
        }
    } else {
        sg_sys_abortf(
            "mismatched pixel format: %s -> %s",
            SG_PIXBUF_FORMATNAME[other->format],
            SG_PIXBUF_FORMATNAME[m_pixbuf.format]);
    }
}

Texture::Texture()
    : tex(0),
      iwidth(0), iheight(0),
      twidth(0), theight(0)
{ }

Texture Texture::load(const std::string &path) {
    Image image;
    image.load(path);
    return Texture::load(image);
}

Texture::Texture(Texture &&other)
    : tex(other.tex),
      iwidth(other.iwidth), iheight(other.iheight),
      twidth(other.twidth), theight(other.theight) {
    scale[0] = other.scale[0];
    scale[1] = other.scale[1];
}

Texture::~Texture() {
    if (tex)
        glDeleteTextures(1, &tex);
}

Texture &Texture::operator=(Texture &&other) {
    if (this == &other)
        return *this;
    if (tex)
        glDeleteTextures(1, &tex);
    tex      = other.tex;
    iwidth   = other.iwidth;
    iheight  = other.iheight;
    twidth   = other.twidth;
    theight  = other.theight;
    scale[0] = other.scale[0];
    scale[1] = other.scale[1];
    other.tex = 0;
    return *this;
}

namespace {

struct FormatInfo {
    GLenum ifmt, fmt, type;
};

const FormatInfo FORMAT_INFO[SG_PIXBUF_NFORMAT] = {
    { GL_R8,    GL_RED,  GL_UNSIGNED_BYTE },
    { GL_RG8,   GL_RG,   GL_UNSIGNED_BYTE },
    { GL_RGBA8, GL_RGB,  GL_UNSIGNED_BYTE },
    { GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 },
    { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE }
};

}

Texture Texture::load(const Image &image) {
    Texture tex;
    auto &info = FORMAT_INFO[image->format];

    tex.iwidth = image->iwidth;
    tex.iheight = image->iheight;
    tex.twidth = image->pwidth;
    tex.theight = image->pheight;
    tex.scale[0] = (float) (1.0 / tex.twidth);
    tex.scale[1] = (float) (1.0 / tex.theight);

    glGenTextures(1, &tex.tex);
    glBindTexture(GL_TEXTURE_2D, tex.tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        info.ifmt,
        tex.twidth,
        tex.theight,
        0,
        info.fmt,
        info.type,
        image->data);
    glBindTexture(GL_TEXTURE_2D, 0);

    sg_opengl_checkerror("Texture::load");
    return tex;
}

Texture Texture::load_1d(const std::string &path) {
    Image image;
    image.load(path);
    return Texture::load_1d(image);
}

Texture Texture::load_1d(const Image &image) {
    Texture tex;
    auto &info = FORMAT_INFO[image->format];

    tex.iwidth = image->iwidth;
    tex.iheight = 1;
    tex.twidth = image->pwidth;
    tex.theight = 1;
    tex.scale[0] = (float) (1.0 / tex.twidth);
    tex.scale[1] = 1.0f;

    glGenTextures(1, &tex.tex);
    glBindTexture(GL_TEXTURE_1D, tex.tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        info.ifmt,
        tex.twidth,
        0,
        info.fmt,
        info.type,
        image->data);
    glBindTexture(GL_TEXTURE_1D, 0);

    sg_opengl_checkerror("Texture::load_1d");
    return tex;
}

}
