/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "minion.hpp"
#include "item.hpp"
#include <algorithm>
namespace Game {

namespace {
const IRect HIT_BOX = IRect::centered(12, 32);
const IRect MEMORY_BOX = IRect::centered(20, 40);
const int JUMP_TIME = 10;
const int JUMP_HOLDTIME = 15;
const int JUMP_TURNTIME = 5;
}

const Walker::Stats Minion::STATS = {
    // ground accel, speed
    1200, 120,
    // air accel, speed
    300, 150,

    // jump time, accel, speed, gravity, doublejump
    25, 400.0f, 200.0f, 650.0f, true,

    // step time
    0.3f
};

Minion::Minion(GameScreen &scr, IVec pos, int direction)
    : Entity(scr, Team::FOE), m_mover(pos), m_walker(),
      m_state(State::WALK), m_statetime(0), m_direction(direction),
      m_haskey(false)
{ }

Minion::~Minion()
{ }

void Minion::update() {
    if (m_screen.is_dreaming())
        return;

    if (m_statetime > 0 && !--m_statetime) {
        if (m_state == State::JUMP_BACK) {
            m_state = State::JUMP;
            m_statetime = JUMP_TIME;
        } else {
            m_state = State::WALK;
        }
    }

    FVec drive((float) m_direction, 0.0f);
    if (m_state == State::JUMP || m_state == State::JUMPING) {
        drive.y = 1.0f;
    }

    unsigned flags = m_walker.update(
        STATS, m_screen.level(), m_mover, drive);
    m_pos = IVec(m_mover.pos());

    if (flags & Walker::FLAG_FOOTSTEP) {
        m_screen.play_sound(Sfx::BOOT, -10.0f, m_mover.pos());
    }
    if (flags & Walker::FLAG_JUMPED) {
        m_screen.play_sound(Sfx::GRUNT, -1.0f, m_mover.pos());
        m_state = State::JUMPING;
        m_statetime = JUMP_HOLDTIME;
    }
    if ((flags & Walker::FLAG_AIRBORNE) == 0 && m_state == State::JUMPING) {
        m_state = State::WALK;
    }
    if (flags & Walker::FLAG_BLOCKED &&
        (flags & Walker::FLAG_AIRBORNE) == 0 &&
        m_state == State::WALK) {
        m_direction = m_direction > 0 ? -1 : +1;
    }

    IRect hitbox = HIT_BOX.offset(m_pos);
    for (auto &ep : m_screen.entities()) {
        Entity &ent = *ep;
        int id = ent.id();
        bool skip = false;
        for (auto &m : m_memory) {
            if (m.id != id)
                continue;
            skip = true;
            if (!m.bounds.contains(m_pos)) {
                // Log::info("FORGET %d", m.id);
                m.forget = true;
            }
            break;
        }

        if (skip)
            continue;

        if (m_state != State::WALK)
            continue;

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

    auto part = std::partition(
        m_memory.begin(), m_memory.end(),
        [](Memory &m) { return !m.forget; });
    m_memory.erase(part, m_memory.end());
}

void Minion::draw(::Graphics::System &gr, int delta) const {
    IVec pos = m_mover.drawpos(delta);

    gr.add_sprite(
        Sprite::KNIGHT_1,
        pos,
        Layer::PHYSICAL,
        m_direction > 0 ?
        Orientation::NORMAL : Orientation::FLIP_HORIZONTAL);
    if (m_haskey) {
        gr.add_sprite(
            Sprite::KEY,
            pos + IVec(0, 24),
            Layer::PHYSICAL);
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
        m_screen.play_sound(Sfx::OPEN, -10.0f, m_mover.pos());
        m_screen.capture_minion();
        break;

    case IType::DOOR_LOCKED:
        if (m_haskey) {
            m_team = Team::DEAD;
            m_screen.play_sound(Sfx::UNLOCK,  -10.0f, m_mover.pos());
            item.set_type(IType::DOOR_OPEN);
            m_screen.capture_minion();
        } else {
            m_screen.play_sound(Sfx::LOCKED, -10.0f, m_mover.pos());
            memorize(item);
        }
        break;

    case IType::KEY:
        if (!m_haskey) {
            m_haskey = true;
            item.destroy();
            m_screen.play_sound(Sfx::PLINK, -10.0f, m_mover.pos());
        }
        break;

    case IType::ACTION:
        memorize(item);
        do_action(item, item.action());
        break;

    case IType::GATEWAY:
    case IType::ADVERSARY:
        break;
    }
}

void Minion::do_action(Item &item, Action action) {
    switch (action) {
    case Action::JUMP:
        m_state = State::JUMP;
        m_statetime = JUMP_TIME;
        break;
    case Action::JUMP_BACK:
        m_screen.play_sound(Sfx::HAA, -10.0f, m_mover.pos());
        m_direction = -m_direction;
        m_state = State::JUMP_BACK;
        m_statetime = JUMP_TURNTIME;
        break;
    case Action::TURN:
        m_screen.play_sound(Sfx::HAA, -10.0f, m_mover.pos());
        m_direction = -m_direction;
        break;
    case Action::DROP:
        if (m_haskey) {
            item.destroy();
            m_haskey = false;
            auto ent = new Item(m_screen, m_pos, Item::Type::KEY);
            m_screen.add_entity(ent);
            memorize(*ent);
        }
        break;
    }
}

void Minion::memorize(Entity &ent) {
    Memory m;
    m.bounds = MEMORY_BOX.offset(ent.pos());
    m.id = ent.id();
    m.forget = false;
    // Log::info("REMEMBER %d", m.id);
    m_memory.push_back(m);
}

}
