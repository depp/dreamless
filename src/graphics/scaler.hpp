/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SCALER_HPP
#define LD_GRAPHICS_SCALER_HPP
#include "sg/opengl.h"
#include "base/array.hpp"
#include "base/image.hpp"
namespace Graphics {
struct CommonData;

struct Scaler {
    int m_width, m_height;
    int m_fwidth, m_fheight;
    GLuint m_tex;
    GLuint m_fbuf;
    Base::Array<float[4]> m_array;
    Base::Texture m_pattern;
    float m_scale[2];

    Scaler();
    void begin(int dest_width, int dest_height,
               int src_width, int src_height);
    void end(const CommonData &com);
};

}
#endif
