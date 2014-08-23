/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_DEFS_HPP
#define LD_GAME_DEFS_HPP
#include "base/vec.hpp"
#include "base/log.hpp"
#include "base/sprite.hpp"
#include "graphics/layer.hpp"
#include "graphics/sprite.hpp"
#include "graphics/system.hpp"
namespace Game {
typedef Base::Log Log;

// Re-export common types.
typedef ::Base::Orientation Orientation;
typedef ::Base::IVec IVec;
typedef ::Base::IRect IRect;
typedef ::Base::FVec FVec;
typedef ::Base::FRect FRect;
typedef ::Graphics::AnySprite AnySprite;
typedef ::Graphics::Sprite Sprite;
typedef ::Graphics::Tile Tile;
typedef ::Graphics::Layer Layer;

struct Defs {
    /// Milliseconds between updates.
    static const int FRAMETIME = 32;

    /// Maximum update lag in milliseconds.
    static const unsigned MAXUPDATE = 500;

    /// Interpolate between positions at successive updates.
    static Base::IVec interp(Base::FVec a, Base::FVec b, int reltime) {
        float frac = reltime * (1.0f / FRAMETIME);
        return Base::IVec(a + (b - a) * frac);
    }

    /// Scale a per-second velocity to a per-update velocity.
    static float velocity(float x) {
        return (1.0f / FRAMETIME) * x;
    }

    /// Scale a per-second acceleration to a per
    static float acceleration(float x) {
        return (1.0f / (FRAMETIME * FRAMETIME)) * x;
    }
};

}
#endif
