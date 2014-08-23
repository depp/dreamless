/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "game_screen.hpp"
#include "base/sprite.hpp"
#include "base/vec.hpp"
#include "graphics/sprite.hpp"
#include "graphics/system.hpp"
namespace Game {

GameScreen::GameScreen(const std::string &name) {
    m_level.load(name);
}

GameScreen::~GameScreen()
{ }

void GameScreen::draw(::Graphics::System &gr, int delta) {
    (void) delta;
    gr.add_sprite(
        Graphics::Sprite::KNIGHT_1,
        Base::IVec(20, 0),
        Base::Orientation::NORMAL);
}

void GameScreen::update(unsigned time, const ControlState &ctl) {
    (void) time;
    (void) &ctl;
}

}
