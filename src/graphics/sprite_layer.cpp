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
    for (int i = 0; i < NLAYER; i++)
        m_array[i].clear();
}

void SpriteLayer::add_sprite(AnySprite sp, Base::IVec pos,
                             Base::Orientation orientation,
                             Layer layer) {
    auto &arr = m_array[static_cast<int>(layer)];
    arr.add(
        m_sheet.get(static_cast<int>(sp)),
        pos.x, pos.y, orientation);
}

void SpriteLayer::upload() {
    for (int i = 0; i < NLAYER; i++)
        m_array[i].upload(GL_DYNAMIC_DRAW);
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

    glUniform4fv(prog->u_vertxform, 1, com.m_xform_world);
    draw_layer(com, Layer::World1);
    draw_layer(com, Layer::Sprite1);
    draw_layer(com, Layer::World2);
    draw_layer(com, Layer::Sprite2);

    glUniform4fv(prog->u_vertxform, 1, com.m_xform_screen);
    draw_layer(com, Layer::Interface);

    glDisableVertexAttribArray(prog->a_vert);
    glUseProgram(0);

    sg_opengl_checkerror("SpriteLayer::draw");
}

void SpriteLayer::draw_layer(const CommonData &com, Layer layer) {
    auto &prog = com.m_sprite;
    auto &arr = m_array[static_cast<int>(layer)];

    if (!arr.empty()) {
        arr.set_attrib(prog->a_vert);
        glDrawArrays(GL_TRIANGLES, 0, arr.size());
    }
}

}
