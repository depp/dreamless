/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "minion.hpp"
#include "item.hpp"
namespace Game {

const Walker::Stats Minion::STATS = {
    // ground accel, speed
    1200, 120,
    // air accel, speed
    300, 150,

    // jump time, accel, speed, gravity, doublejump
    25, 400.0f, 180.0f, 600.0f, true,

    // step time
    0.3
};

Minion::Minion(GameScreen &scr, IVec pos)
    : Entity(scr, Team::FOE), m_mover(pos), m_walker(), m_direction(1)
{ }

Minion::~Minion()
{ }

void Minion::update(unsigned time) {
    (void) time;

    unsigned flags = m_walker.update(
        STATS, m_screen.level(),
        m_mover,
        FVec((float) m_direction, 0.0f));
    if (flags & Walker::FLAG_BLOCKED) {
        m_direction = m_direction > 0 ? -1 : +1;
    }
    if (flags & Walker::FLAG_FOOTSTEP) {
        m_screen.play_sound(time + Defs::FRAMETIME,
                            Sfx::BOOT, m_mover.pos(), -10.0f);
    }

    m_pos = IVec(m_mover.pos());
    IRect hitbox = IRect::centered(8, 24).offset(m_pos);
    for (auto &ep : m_screen.entities()) {
        Entity &ent = *ep;
        switch (ent.team()) {
        case Team::INTERACTIVE:
            if (hitbox.contains(ent.pos())) {
                Item *item = dynamic_cast<Item *>(&ent);
                if (item)
                    hit_item(*item);
            }
            break;

        default:
            break;
        }
    }
}

void Minion::draw(::Graphics::System &gr, int delta) const {
    gr.add_sprite(
        Sprite::KNIGHT_1,
        m_mover.drawpos(delta),
        m_direction > 0 ?
        Orientation::NORMAL : Orientation::FLIP_HORIZONTAL,
        Layer::Sprite1);
}

void Minion::hit_item(Item &item) {
    typedef Item::Type IType;
    switch (item.type()) {
    case IType::DOOR_OPEN:
        m_team = Team::DEAD;
        break;

    case IType::DOOR_CLOSED:
        m_team = Team::DEAD;
        item.set_type(IType::DOOR_OPEN);
        break;

    case IType::DOOR_LOCKED:
        // try to unlock
        break;

    case IType::KEY:
        // pick up
        break;

    case IType::ACTION_JUMP:
        // perform action
        break;
    case IType::ACTION_JUMPBACK:
        // perform action
        break;

    case IType::ACTION_TURN:
        // perform action
        break;
    }
}

}
