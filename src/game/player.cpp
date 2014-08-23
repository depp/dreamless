/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "player.hpp"
namespace Game {

Player::Player(GameScreen &scr, IVec pos)
    : Entity(scr, Team::FRIEND) {
    m_pos = (FVec) pos;
    m_pos0 = m_pos;
}

Player::~Player()
{ }

void Player::update(unsigned time) {
    (void) time;

    m_pos0 = m_pos;
    float xaxis = m_screen.control().get_xaxis();
    // float yaxis = m_screen.control().get_yaxis();
    m_pos.x += xaxis * Defs::velocity(32 * 5);
}

void Player::draw(::Graphics::System &gr, int delta) {
    gr.add_sprite(
        Sprite::KNIGHT_1,
        Defs::interp(m_pos0, m_pos, delta),
        Orientation::NORMAL,
        Layer::Sprite1);
}

}
