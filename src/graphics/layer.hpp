/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_LAYER_HPP
#define LD_GRAPHICS_LAYER_HPP
namespace Graphics {

/// Layers for rendering sprites.
enum class Layer {
    /// Tiles in the physical wold.
    World1,

    /// Tiles in the dream world.
    World2,

    /// Sprites in the physical world.
    Sprite1,

    /// Sprites in the dream world.
    Sprite2,

    /// User interface elements (screen-relative).
    Interface
};

static const int NLAYER = 5;

}
#endif
