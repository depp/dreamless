/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_SCREEN_HPP
#define LD_GAME_SCREEN_HPP
namespace Graphics {
class System;
}
namespace Game {
class ControlState;

class Screen {
public:
    Screen();
    Screen(const Screen &) = delete;
    Screen(Screen &&) = delete;
    virtual ~Screen();
    Screen &operator=(const Screen &) = delete;
    Screen &operator=(Screen &&) = delete;
    virtual void draw(::Graphics::System &gr, int delta) = 0;
    virtual void update(unsigned time, const ControlState &ctl);
};

}
#endif
