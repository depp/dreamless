/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "shader.hpp"
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
    FIELD(u_vertoff),
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

#define TYPE Scale
const ShaderField Scale::UNIFORMS[] = {
    FIELD(u_picture),
    FIELD(u_pattern),
    FIELD(u_pixscale),
    { nullptr, 0 }
};

const ShaderField Scale::ATTRIBUTES[] = {
    FIELD(a_vert),
    { nullptr, 0 }
};
#undef TYPE

#define TYPE Dream
const ShaderField Dream::UNIFORMS[] = {
    FIELD(u_reality),
    FIELD(u_noise),
    FIELD(u_world),
    FIELD(u_color),
    FIELD(u_blendscale),
    FIELD(u_noisescale),
    FIELD(u_noiseoffset),
    { nullptr, 0 }
};

const ShaderField Dream::ATTRIBUTES[] = {
    FIELD(a_vert),
    { nullptr, 0 }
};
#undef TYPE

}
