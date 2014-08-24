/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "player.hpp"
#include "action.hpp"
namespace Game {

#if 0
const Walker::Stats Player::STATS = {
    // ground accel, speed
    1200, 150,
    // air accel, speed
    300, 180,

    // jump time, accel, speed, gravity, doublejump
    25, 400.0f, 280.0f, 1200.0f, false,

    // step time
    0.16
};
#else
const Walker::Stats Player::STATS = {
    // ground accel, speed
    600, 130,
    // air accel, speed
    500, 180,

    // jump time, accel, speed, gravity, doublejump
    25, 400.0f, 280.0f, 800.0f, true,

    // step time
    0.20
};
#endif
Player::Player(GameScreen &scr, IVec pos)
    : Entity(scr, Team::FRIEND), m_mover(pos), m_walker(),
      m_selection(0)
{ }

Player::~Player()
{ }

void Player::update() {
    auto &ctl = m_screen.control();
    unsigned flags = m_walker.update(
        STATS, m_screen.level(),
        m_mover, ctl.get_2d());
    m_screen.set_camera(m_mover.pos());
    m_pos = IVec(m_mover.pos());
    if (flags & Walker::FLAG_FOOTSTEP) {
        m_screen.play_sound(Sfx::FOOT, -10.0f, m_mover.pos());
    }

    int move = 0;
    if (ctl.get_button_instant(Button::NEXT))
        move++;
    if (ctl.get_button_instant(Button::PREV))
        move--;
    if (move != 0)
        m_screen.play_sound(Sfx::CLICK, -10.0f);
    if (move > 0) {
        m_selection++;
        if (m_selection >= ACTION_COUNT)
            m_selection = 0;
    } else if (move < 0) {
        m_selection--;
        if (m_selection < 0)
            m_selection = ACTION_COUNT - 1;
    }
}

void Player::draw(::Graphics::System &gr, int delta) const {
    gr.add_sprite(
        Sprite::KNIGHT_1,
        m_mover.drawpos(delta),
        Orientation::NORMAL,
        Layer::SPRITE_1);

    IVec center(Defs::WIDTH / 2, Defs::HEIGHT - 20);
    for (int i = 0; i < ACTION_COUNT; i++) {
        IVec pos = center + IVec(18 * (1 - ACTION_COUNT + 2 * i), 0);
        gr.add_sprite(
            action_sprite(static_cast<Action>(i)),
            pos,
            Orientation::NORMAL,
            Layer::INTERFACE);
        if (m_selection == i) {
            gr.add_sprite(
                Sprite::SELECTION,
                pos,
                Orientation::NORMAL,
                Layer::INTERFACE);
        }
    }
}

}
