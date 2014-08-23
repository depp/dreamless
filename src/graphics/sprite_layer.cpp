/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "common.hpp"
#include "sprite_layer.hpp"
#include "sprite.hpp"
namespace Graphics {

SpriteLayer::SpriteLayer()
    : m_sheet("", SPRITES)
{ }

void SpriteLayer::clear() {
    m_world.clear();
    m_screen.clear();
}

void SpriteLayer::upload() {
    m_world.upload(GL_DYNAMIC_DRAW);
    m_screen.upload(GL_DYNAMIC_DRAW);
}

void SpriteLayer::draw(const CommonData &com) {
    auto &prog = com.m_sprite;

    glUseProgram(prog.prog());
    glEnableVertexAttribArray(prog->a_vert);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sheet.texture());

    glUniform2fv(prog->u_texscale, 1, m_sheet.texscale());
    glUniform1i(prog->u_texture, 0);

    if (!m_world.empty()) {
        m_world.set_attrib(prog->a_vert);
        glUniform4fv(prog->u_vertxform, 1, com.m_xform_world);
        glDrawArrays(GL_TRIANGLES, 0, m_world.size());
    }

    if (!m_screen.empty()) {
        m_screen.set_attrib(prog->a_vert);
        glUniform4fv(prog->u_vertxform, 1, com.m_xform_screen);
        glDrawArrays(GL_TRIANGLES, 0, m_screen.size());
    }

    glDisableVertexAttribArray(prog->a_vert);
    glUseProgram(0);

    sg_opengl_checkerror("SpriteLayer::draw");
}

}
