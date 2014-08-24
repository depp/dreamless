/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SHADER_HPP
#define LD_GRAPHICS_SHADER_HPP
#include "base/shader.hpp"
namespace Shader {

/// Uniforms and attributes for the "plain" shader.
struct Plain {
    static const Base::ShaderField UNIFORMS[];
    static const Base::ShaderField ATTRIBUTES[];

    GLint a_vert;
    GLint u_vertxform;
    GLint u_color;
};

/// Uniforms and attributes for the "sprite" shader.
struct Sprite {
    static const Base::ShaderField UNIFORMS[];
    static const Base::ShaderField ATTRIBUTES[];

    GLint a_vert;
    GLint u_vertxform;
    GLint u_texscale;
    GLint u_texture;
};

/// Uniforms and attributes for the "text" shader.
struct Text {
    static const Base::ShaderField UNIFORMS[];
    static const Base::ShaderField ATTRIBUTES[];

    GLint a_vert;
    GLint u_vertxform;
    GLint u_vertoff;
    GLint u_texscale;
    GLint u_texture;
    GLint u_color;
};

/// Uniforms and attributes for the "scal" shader.
struct Scale {
    static const Base::ShaderField UNIFORMS[];
    static const Base::ShaderField ATTRIBUTES[];

    GLint a_vert;

    GLint u_picture;
    GLint u_pattern;
    GLint u_pixscale;
};

/// Uniforms and attributes for the "dream" shader.
struct Dream {
    static const Base::ShaderField UNIFORMS[];
    static const Base::ShaderField ATTRIBUTES[];

    GLint a_vert;

    GLint u_reality;
    GLint u_noise;
    GLint u_background;

    GLint u_world;
    GLint u_color;
    GLint u_blendscale;
    GLint u_noisescale;
    GLint u_noiseoffset;
    GLint u_backgroundxform;
};

}
#endif
