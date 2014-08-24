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
    gr.add_sprite(
        sprite(), m_pos, Orientation::NORMAL, Layer::SPRITE_1);
}

Sprite Item::sprite() const {
    switch (m_type) {
    case Type::DOOR_OPEN:
        return Sprite::DOOR_OPEN;
    case Type::DOOR_CLOSED:
        return Sprite::DOOR_CLOSED;
    case Type::DOOR_LOCKED:
        return Sprite::DOOR_LOCKED;
    case Type::KEY:
        return Sprite::KEY;
    case Type::ACTION:
        return action_sprite(m_action);
    }

    Log::abort("invalid item type");
    return Sprite::DOOR_OPEN;
}

}
