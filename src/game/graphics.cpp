/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "graphics.hpp"
#include "graphics_common.hpp"
#include "graphics_sprite.hpp"
#include "sprite.hpp"
namespace Graphics {

namespace {
void make_xform(float *xform, int w, int h, int x, int y) {
    xform[0] = 2.0 / w;
    xform[1] = 2.0 / h;
    xform[2] = x * (-1.0 / (w / 2));
    xform[3] = y * (-1.0 / (h / 2));
}
}

System::System()
    : m_common(new CommonData),
      m_sprite(new SpriteData)
{ }

System::~System()
{ }

void System::begin(int width, int height) {
    m_width = width;
    m_height = height;
    m_sprite->clear();
}

void System::end() {
    m_sprite->upload();
}

void System::draw() {
    make_xform(m_common->m_xform_world, m_width, m_height, 0, 0);
    make_xform(m_common->m_xform_screen, m_width, m_height, 0, 0);

    glViewport(0, 0, m_width, m_height);
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_sprite->draw(*m_common);
}

void System::add_sprite(AnySprite sp, int x, int y,
                        Base::Orientation orientation,
                        bool screen) {
    auto &arr = screen ? m_sprite->m_screen : m_sprite->m_world;
    arr.add(
        m_sprite->m_sheet.get(static_cast<int>(sp)),
        x, y, orientation);
}

}
