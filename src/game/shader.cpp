/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "graphics_common.hpp"
using Base::ShaderField;
namespace Shader {

#define FIELD(n) { #n, offsetof(TYPE, n) }

#define TYPE Plain
const ShaderField Plain::UNIFORMS[] = {
    FIELD(u_vertxform),
    FIELD(u_color),
    { nullptr, 0 }
};

const ShaderField Plain::ATTRIBUTES[] = {
    FIELD(a_vert),
    { nullptr, 0 }
};
#undef TYPE

#define TYPE Sprite
const ShaderField Sprite::UNIFORMS[] = {
    FIELD(u_vertxform),
    FIELD(u_texscale),
    FIELD(u_texture),
    { nullptr, 0 }
};

const ShaderField Sprite::ATTRIBUTES[] = {
    FIELD(a_vert),
    { nullptr, 0 }
};
#undef TYPE

#define TYPE Text
const ShaderField Text::UNIFORMS[] = {
    FIELD(u_vertxform),
    FIELD(u_texscale),
    FIELD(u_texture),
    FIELD(u_color),
    { nullptr, 0 }
};

const ShaderField Text::ATTRIBUTES[] = {
    FIELD(a_vert),
    { nullptr, 0 }
};
#undef TYPE

#define TYPE Television
const ShaderField Television::UNIFORMS[] = {
    FIELD(u_picture),
    FIELD(u_pattern),
    FIELD(u_banding),
    FIELD(u_noise),
    FIELD(u_noiseoffset),
    FIELD(u_texscale),
    FIELD(u_color),
    { nullptr, 0 }
};

const ShaderField Television::ATTRIBUTES[] = {
    FIELD(a_vert),
    { nullptr, 0 }
};
#undef TYPE

}
