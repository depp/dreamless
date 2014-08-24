/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_LAYER_HPP
#define LD_GRAPHICS_LAYER_HPP
namespace Graphics {

/// Layers for rendering sprites.
enum class Layer {
    /// Tiles in the physical wold.
    TILE_1,

    /// Tiles in the dream world.
    TILE_2,

    /// Sprites in the physical world.
    SPRITE_1,

    /// Sprites in the dream world.
    SPRITE_2,

    /// User interface elements (screen-relative).
    INTERFACE
};

static const int LAYER_COUNT = 5;

}
#endif
