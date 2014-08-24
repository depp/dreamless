/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "item.hpp"
namespace Game {

Item::Item(GameScreen &scr, IVec pos, Type type)
    : Entity(scr, Team::INTERACTIVE), m_type(type),
      m_action(Action::JUMP) {
    m_pos = pos;
}

Item::~Item()
{ }

void Item::draw(::Graphics::System &gr, int delta) const {
    (void) delta;
    switch (m_type) {
    case Type::DOOR_OPEN:
        gr.add_sprite(Sprite::DOOR_OPEN, m_pos, Layer::PHYSICAL);
        break;

    case Type::DOOR_CLOSED:
        gr.add_sprite(Sprite::DOOR_CLOSED, m_pos, Layer::PHYSICAL);
        break;

    case Type::DOOR_LOCKED:
        gr.add_sprite(Sprite::DOOR_LOCKED, m_pos, Layer::PHYSICAL);
        break;

    case Type::KEY:
        gr.add_sprite(Sprite::KEY, m_pos, Layer::PHYSICAL);
        break;

    case Type::ACTION:
        gr.add_sprite(action_sprite(m_action), m_pos, Layer::BOTH);
        break;

    case Type::GATEWAY:
        gr.add_sprite(Sprite::PORTAL, m_pos, Layer::DREAM);
        break;
    }
}

}
