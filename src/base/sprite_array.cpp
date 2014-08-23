/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sprite.hpp"
namespace Base {

SpriteArray::SpriteArray()
{ }

SpriteArray::SpriteArray(SpriteArray &&other)
    : m_array(std::move(other.m_array))
{ }

SpriteArray::~SpriteArray()
{ }

void SpriteArray::clear() {
    m_array.clear();
}

void SpriteArray::add(SpriteRect tex, int x, int y) {
    short (*data)[4] = m_array.insert(6);

    short tx0 = tex.x, tx1 = tex.x + tex.w;
    short ty1 = tex.y, ty0 = tex.y + tex.h;
    short vx0 = x, vx1 = x + tex.w;
    short vy0 = y, vy1 = y + tex.h;

    data[0][0] = vx0; data[0][1] = vy0; data[0][2] = tx0; data[0][3] = ty0;
    data[1][0] = vx1; data[1][1] = vy0; data[1][2] = tx1; data[1][3] = ty0;
    data[2][0] = vx0; data[2][1] = vy1; data[2][2] = tx0; data[2][3] = ty1;
    data[3][0] = vx0; data[3][1] = vy1; data[3][2] = tx0; data[3][3] = ty1;
    data[4][0] = vx1; data[4][1] = vy0; data[4][2] = tx1; data[4][3] = ty0;
    data[5][0] = vx1; data[5][1] = vy1; data[5][2] = tx1; data[5][3] = ty1;
}

void SpriteArray::add(SpriteRect tex, int x, int y, Orientation orient) {
    short (*data)[4] = m_array.insert(6);

    short tx0 = tex.x, tx1 = tex.x + tex.w;
    short ty1 = tex.y, ty0 = tex.y + tex.h;

    data[0][2] = tx0; data[0][3] = ty0;
    data[1][2] = tx1; data[1][3] = ty0;
    data[2][2] = tx0; data[2][3] = ty1;
    data[3][2] = tx0; data[3][3] = ty1;
    data[4][2] = tx1; data[4][3] = ty0;
    data[5][2] = tx1; data[5][3] = ty1;

    short rx0 = -tex.cx, rx1 = tex.w - tex.cx;
    short ry0 = -tex.cy, ry1 = tex.h - tex.cy;
    short vx[4], vy[4];
    switch (orient) {
    case Orientation::NORMAL:
        vx[0] = vx[2] = x + rx0;
        vx[1] = vx[3] = x + rx1;
        vy[0] = vy[1] = y + ry0;
        vy[2] = vy[3] = y + ry1;
        break;

    case Orientation::ROTATE_90:
        vx[2] = vx[3] = x + ry0;
        vx[0] = vx[1] = x + ry1;
        vy[2] = vy[0] = y + rx0;
        vy[3] = vy[1] = y + rx1;
        break;

    case Orientation::ROTATE_180:
        vx[3] = vx[1] = x + rx0;
        vx[2] = vx[0] = x + rx1;
        vy[3] = vy[2] = y + ry0;
        vy[1] = vy[0] = y + ry1;
        break;

    case Orientation::ROTATE_270:
        vx[1] = vx[0] = x + ry0;
        vx[3] = vx[2] = x + ry1;
        vy[1] = vy[3] = y + rx0;
        vy[0] = vy[2] = y + rx1;
        break;

    case Orientation::FLIP_VERTICAL:
        vx[0] = vx[2] = x + rx0;
        vx[1] = vx[3] = x + rx1;
        vy[0] = vy[1] = y + ry1;
        vy[2] = vy[3] = y + ry0;
        break;

    case Orientation::TRANSPOSE_2:
        vx[2] = vx[3] = x + ry0;
        vx[0] = vx[1] = x + ry1;
        vy[2] = vy[0] = y + rx1;
        vy[3] = vy[1] = y + rx0;
        break;

    case Orientation::FLIP_HORIZONTAL:
        vx[3] = vx[1] = x + rx0;
        vx[2] = vx[0] = x + rx1;
        vy[3] = vy[2] = y + ry1;
        vy[1] = vy[0] = y + ry0;
        break;

    case Orientation::TRANSPOSE:
        vx[1] = vx[0] = x + ry0;
        vx[3] = vx[2] = x + ry1;
        vy[1] = vy[3] = y + rx1;
        vy[0] = vy[2] = y + rx0;
        break;

    default:
#ifdef __GCC__
        __builtin_unreachable();
#endif
        return;
    }

    data[0][0] = vx[0]; data[0][1] = vy[0];
    data[1][0] = vx[1]; data[1][1] = vy[1];
    data[2][0] = vx[2]; data[2][1] = vy[2];
    data[3][0] = vx[2]; data[3][1] = vy[2];
    data[4][0] = vx[1]; data[4][1] = vy[1];
    data[5][0] = vx[3]; data[5][1] = vy[3];
}

void SpriteArray::upload(GLuint usage) {
    m_array.upload(usage);
}

void SpriteArray::set_attrib(GLint attrib) {
    m_array.set_attrib(attrib);
}

}
