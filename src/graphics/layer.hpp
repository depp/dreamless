/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_LAYER_HPP
#define LD_GRAPHICS_LAYER_HPP
namespace Graphics {

/// Layers for rendering sprites.
enum class Layer {
    /// Level tiles.
    TILE,

    /// Objects in the physical world.
    PHYSICAL,

    /// Objects in the dream world.
    DREAM,

    /// Objects in both worlds.
    BOTH,

    /// User interface elements (screen-relative).
    INTERFACE
};

static const int LAYER_COUNT = 5;

}
#endif
