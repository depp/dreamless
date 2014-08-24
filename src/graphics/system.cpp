/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "system.hpp"
#include "common.hpp"
#include "scaler.hpp"
#include "sprite_layer.hpp"
#include "sprite.hpp"
#include "base/vec.hpp"
#include "base/log.hpp"
namespace Graphics {

namespace {
void make_xform(float *xform, int w, int h, int x, int y) {
    xform[0] = 2.0 / w;
    xform[1] = 2.0 / h;
    xform[2] = -1.0 - xform[0] * x;
    xform[3] = -1.0 - xform[1] * y;
}
}

System::System()
    : m_common(new CommonData),
      m_sprite(new SpriteLayer),
      m_scaler(new Scaler)
{ }

System::~System()
{ }

void System::begin(int width, int height) {
    m_width = width;
    m_height = height;
    m_sprite->clear(false);
}

void System::end() {
    m_sprite->upload();
}

void System::draw() {
    int width = (m_width + 1) / 2, height = (m_height + 1) / 2;
    m_common->m_blendcolor = Color::palette(3);
    m_scaler->begin(m_width, m_height, width, height);

    make_xform(m_common->m_xform_world,
               width, height, m_camera.x, m_camera.y);
    make_xform(m_common->m_xform_screen,
               width, height, 0, 0);

    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_sprite->draw(*m_common);
    m_scaler->end(*m_common);
}

void System::set_camera(Base::IVec pos) {
    m_camera = pos;
}

void System::add_sprite(AnySprite sp, Base::IVec pos,
                        Base::Orientation orientation,
                        Layer layer) {
    m_sprite->add_sprite(sp, pos, orientation, layer);
}

}
