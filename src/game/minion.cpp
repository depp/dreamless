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
    : Entity(scr, Team::FOE), m_mover(pos), m_walker(), m_direction(1),
      m_haskey(false)
{ }

Minion::~Minion()
{ }

void Minion::update() {
    unsigned flags = m_walker.update(
        STATS, m_screen.level(),
        m_mover,
        FVec((float) m_direction, 0.0f));
    if (flags & Walker::FLAG_BLOCKED) {
        m_direction = m_direction > 0 ? -1 : +1;
    }
    if (flags & Walker::FLAG_FOOTSTEP) {
        m_screen.play_sound(Sfx::BOOT, m_mover.pos(), -10.0f);
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
    IVec pos = m_mover.drawpos(delta);

    gr.add_sprite(
        Sprite::KNIGHT_1,
        pos,
        m_direction > 0 ?
        Orientation::NORMAL : Orientation::FLIP_HORIZONTAL,
        Layer::SPRITE_1);
    if (m_haskey) {
        gr.add_sprite(
            Sprite::KEY,
            pos + IVec(0, 24),
            Orientation::NORMAL,
            Layer::SPRITE_1);
    }
}

void Minion::hit_item(Item &item) {
    typedef Item::Type IType;
    switch (item.type()) {
    case IType::DOOR_OPEN:
        break;

    case IType::DOOR_CLOSED:
        m_team = Team::DEAD;
        item.set_type(IType::DOOR_OPEN);
        m_screen.play_sound(Sfx::OPEN, m_mover.pos(), -10.0f);
        break;

    case IType::DOOR_LOCKED:
        if (m_haskey) {
            m_team = Team::DEAD;
            m_screen.play_sound(Sfx::UNLOCK, m_mover.pos(), -10.0f);
            item.set_type(IType::DOOR_OPEN);
        } else {
            m_screen.play_sound(Sfx::LOCKED, m_mover.pos(), -10.0f);
        }
        break;

    case IType::KEY:
        if (!m_haskey) {
            m_haskey = true;
            item.destroy();
            m_screen.play_sound(Sfx::PLINK, m_mover.pos(), -10.0f);
        }
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
