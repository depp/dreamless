/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "player.hpp"
namespace Game {

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

Player::Player(GameScreen &scr, IVec pos)
    : Entity(scr, Team::FRIEND), m_mover(pos), m_walker()
{ }

Player::~Player()
{ }

void Player::update() {
    unsigned flags = m_walker.update(
        STATS, m_screen.level(),
        m_mover, m_screen.control().get_2d());
    m_screen.set_camera(m_mover.pos());
    m_pos = IVec(m_mover.pos());
    if (flags & Walker::FLAG_FOOTSTEP) {
        m_screen.play_sound(Sfx::FOOT, m_mover.pos(), -10.0f);
    }
}

void Player::draw(::Graphics::System &gr, int delta) const {
    gr.add_sprite(
        Sprite::KNIGHT_1,
        m_mover.drawpos(delta),
        Orientation::NORMAL,
        Layer::Sprite1);
}

}
