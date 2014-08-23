/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SPRITE_HPP
#define LD_GRAPHICS_SPRITE_HPP
namespace Base {
struct Sprite;
}
namespace Graphics {

// Define the "sprite" and "tile" enumerations.
#include "sprite_enum.hpp"

/// A sprite from any category.
class AnySprite {
private:
    int m_index;

public:
    AnySprite() { }
    AnySprite(Sprite s) : m_index(static_cast<int>(s)) { }
    AnySprite(Tile s) : m_index(static_cast<int>(s) + SPRITE_COUNT) { }
    operator int() const { return m_index; }
};

extern const ::Base::Sprite SPRITES[];

}
#endif
