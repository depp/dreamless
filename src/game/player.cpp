/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "player.hpp"
#include "action.hpp"
#include "item.hpp"
#include "graphics/color.hpp"
namespace Game {

namespace {
const int DIALOGUE_STICKY = 25;
const int DIALOGUE_DISMISS = 32 * 20;
const int GIRL_COLOR = 20;
const int SHADOW_COLOR = 28;
};

const Walker::Stats Player::STATS_PHYSICAL = {
    // ground accel, speed
    1200, 150,
    // air accel, speed
    300, 180,

    // jump time, accel, speed, gravity, doublejump
    25, 400.0f, 280.0f, 1200.0f, false,

    // step time
    0.16f
};

const Walker::Stats Player::STATS_DREAM = {
    // ground accel, speed
    600, 130,
    // air accel, speed
    500, 180,

    // jump time, accel, speed, gravity, doublejump
    25, 400.0f, 300.0f, 800.0f, true,

    // step time
    0.20f
};

Player::Player(GameScreen &scr, IVec pos)
    : Entity(scr, Team::FRIEND), m_mover(pos), m_walker(),
      m_selection(0), m_num_actions(0), m_direction(1),
      m_dialogue(0) {
    const Level &level = scr.level();
    for (int i = 0; i < ACTION_COUNT; i++) {
        auto a = static_cast<Action>(i);
        if (level.is_action_allowed(a))
            m_actions[m_num_actions++] = a;
    }
}

Player::~Player()
{ }

void Player::update() {
    auto &ctl = m_screen.control();
    FVec drive = ctl.get_2d();
    unsigned flags = m_walker.update(
        STATS_DREAM, // m_screen.is_dreaming() ? STATS_DREAM : STATS_PHYSICAL,
        m_screen.level(), m_mover, drive);
    if (drive.x < -0.5f)
        m_direction = -1;
    else if (drive.x > 0.5f)
        m_direction = 1;
    m_screen.set_camera(m_mover.pos());
    m_pos = IVec(m_mover.pos());
    if (flags & Walker::FLAG_FOOTSTEP) {
        m_screen.play_sound(Sfx::FOOT, -10.0f, m_mover.pos());
    }

    if (!m_screen.is_dreaming())
        return;

    IRect hitbox = IRect::centered(12, 24).offset(m_pos);
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

    if (m_dialogue > 0) {
        m_screen.set_camera(m_mover.pos() + FVec(0, -80), true);
        m_dialogue_time++;
        if (m_dialogue_time >= DIALOGUE_STICKY) {
            bool dismiss = m_dialogue_time >= DIALOGUE_DISMISS ||
                ctl.get_button_instant(Button::NEXT) ||
                ctl.get_button_instant(Button::PREV) ||
                ctl.get_button_instant(Button::ACTION) ||
                ctl.get_button_instant(Button::ESCAPE);
            if (dismiss)
                show_dialogue(m_dialogue + 1);
        }
    } else if (m_num_actions > 0) {
        int move = 0;
        if (ctl.get_button_instant(Button::NEXT))
            move++;
        if (ctl.get_button_instant(Button::PREV))
            move--;
        if (move != 0)
            m_screen.play_sound(Sfx::CLICK, -10.0f);
        if (move > 0) {
            m_selection++;
            if (m_selection >= m_num_actions)
                m_selection = 0;
        } else if (move < 0) {
            m_selection--;
            if (m_selection < 0)
                m_selection = m_num_actions - 1;
        }

        if (ctl.get_button_instant(Button::ACTION)) {
            m_screen.analytics().action_count++;
            m_screen.play_sound(Sfx::WAP, -10.0f, m_mover.pos());
            auto ent = new Item(m_screen, m_pos, Item::Type::ACTION);
            ent->set_action(m_actions[m_selection]);
            m_screen.add_entity(ent);
        }
    }
}

void Player::draw(::Graphics::System &gr, int delta) const {
    gr.add_sprite(
        Sprite::GIRL,
        m_mover.drawpos(delta),
        Layer::BOTH,
        m_direction > 0 ?
        Orientation::NORMAL : Orientation::FLIP_HORIZONTAL);

    if (m_screen.is_dreaming() && m_num_actions > 0) {
        IVec center(Defs::WIDTH / 2, Defs::HEIGHT - 20);
        for (int i = 0; i < m_num_actions; i++) {
            IVec pos = center + IVec(18 * (1 - m_num_actions + 2 * i), 0);
            gr.add_sprite(
                action_sprite(m_actions[i]),
                pos,
                Layer::INTERFACE);
            if (m_selection == i) {
                gr.add_sprite(
                    Sprite::SELECTION,
                    pos,
                    Layer::INTERFACE);
            }
        }
    }

    if (m_dialogue > 0) {
        auto &line = m_screen.level().dialogue()[m_dialogue - 1];
        const int PWIDTH = 128;
        const int MARGIN = 16;
        const int CENTER = Defs::WIDTH / 2;
        gr.add_sprite(
            Sprite::DIALOG,
            IVec(CENTER, MARGIN + PWIDTH / 2),
            Layer::INTERFACE);
        gr.add_sprite(
            line.speaker == 0 ? Sprite::TALKG1 : Sprite::TALKS1,
            IVec(CENTER - 256 + MARGIN + PWIDTH / 2,
                 MARGIN + PWIDTH / 2),
            Layer::INTERFACE);
        gr.put_text(
            IVec(CENTER - 256 + PWIDTH + MARGIN * 2, PWIDTH + MARGIN),
            Graphics::HAlign::LEFT,
            Graphics::VAlign::TOP,
            512 - MARGIN * 3 - PWIDTH,
            Graphics::Color::palette(
                line.speaker == 0 ? GIRL_COLOR : SHADOW_COLOR),
            line.text);
    }
}

void Player::hit_item(Item &item) {
    typedef Item::Type IType;
    switch (item.type()) {
    case IType::DOOR_OPEN:
    case IType::DOOR_CLOSED:
    case IType::DOOR_LOCKED:
    case IType::KEY:
    case IType::ACTION:
        break;

    case IType::GATEWAY:
        m_screen.wake_up();
        break;

    case IType::ADVERSARY:
        if (m_dialogue == 0)
            show_dialogue(1);
        break;
    }
}

void Player::show_dialogue(int index) {
    int limit = m_screen.level().dialogue().size();
    m_screen.analytics().talked_to_shadow = true;
    if (index > limit) {
        m_dialogue = -1;
    } else {
        m_dialogue = index;
        m_dialogue_time = 0;
    }
}

}
