/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_COMMON_HPP
#define LD_GRAPHICS_COMMON_HPP
#include "shader.hpp"
namespace Graphics {

struct CommonData {
    Base::Program<Shader::Sprite> m_sprite;

    float m_xform_world[4];
    float m_xform_screen[4];

    CommonData();
};

}
#endif
