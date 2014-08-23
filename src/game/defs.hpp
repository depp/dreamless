/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_DEFS_HPP
#define LD_GAME_DEFS_HPP
#include "base/vec.hpp"
class FVec;
namespace Game {

struct Defs {
    /// Milliseconds between updates.
    static const int FRAMETIME = 32;

    /// Maximum update lag in milliseconds.
    static const unsigned MAXUPDATE = 500;

    /// Interpolate between positions at successive updates.
    static Base::FVec interp(Base::FVec a, Base::FVec b, int reltime) {
        float frac = reltime * (1.0f / FRAMETIME);
        return a + (b - a) * frac;
    }

};

}
#endif
