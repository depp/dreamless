/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_GAME_SCREEN_HPP
#define LD_GAME_GAME_SCREEN_HPP
#include "level.hpp"
#include "screen.hpp"
#include <string>
namespace Game {

class GameScreen : public Screen {
    Level m_level;

public:
    explicit GameScreen(const std::string &name);
    virtual ~GameScreen();

    virtual void draw(::Graphics::System &gr, int delta);
    virtual void update(unsigned time, const ControlState &ctl);
};

}
#endif
