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
#include <cmath>
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
    /// Screen width.
    static const int WIDTH = 1280 / 2;

    /// Screen height.
    static const int HEIGHT = 720 / 2;

    /// Milliseconds between updates.
    static const int FRAMETIME = 32;

    /// Maximum update lag in milliseconds.
    static const unsigned MAXUPDATE = 500;

    /// The log 2 of the size of a tile.
    static const int TILEBITS = 5;

    /// The size of a tile.
    static const int TILESZ = 1 << TILEBITS;

    /// Get the timestep size (seconds per step).
    static float dt() { return (float) ((double) FRAMETIME * 1e-3); }

    /// Get the squared timestep size (seconds squared per step
    /// squared).
    static float dt2() {
        return (float) ((double) (FRAMETIME * FRAMETIME) * 1e-6);
    }

    /// Get inverse timestep size (steps per second).
    static float invdt() { return (float) (1e3 / (double) FRAMETIME); }

    /// Get inverse timestep size (steps squared per second squared).
    static float invdt2() {
        return (float) (1e6 / (double) (FRAMETIME * FRAMETIME));
    }

    /// Interpolate between positions at successive updates.
    static Base::IVec interp(Base::FVec a, Base::FVec b, int reltime) {
        float frac = reltime * (1.0f / FRAMETIME);
        return Base::IVec(a + (b - a) * frac);
    }

    /// Get the tile position for a pixel position.
    static IVec tile_pos(IVec pos) {
        return IVec(pos.x >> TILEBITS, pos.y >> TILEBITS);
    }

    /// Get the tile position for a pixel position.
    static IVec tile_pos(FVec pos) {
        return tile_pos(IVec(pos));
    }

    /// Get the position relative to the tile lower left.
    static IVec tile_relpos(IVec pos) {
        return IVec(pos.x & (TILESZ - 1), pos.y & (TILESZ - 1));
    }

    /// Get the position relative to the tile lower left.
    static FVec tile_relpos(FVec pos) {
        // Note: this won't be correct outside the first quadrant.
        // But the level is all within the first quadrant, so it
        // doesn't matter.
        return FVec(
			std::fmod(pos.x, static_cast<float>(TILESZ)),
			std::fmod(pos.y, static_cast<float>(TILESZ)));
    }
};

}
#endif
