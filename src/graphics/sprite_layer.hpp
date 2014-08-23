/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SPRITE_LAYER_HPP
#define LD_GRAPHICS_SPRITE_LAYER_HPP
#include "base/sprite.hpp"
namespace Graphics {
class CommonData;

struct SpriteLayer {
    Base::SpriteSheet m_sheet;
    Base::SpriteArray m_world;
    Base::SpriteArray m_screen;

    SpriteLayer();

    void clear();
    void upload();
    void draw(const CommonData &com);
};

}
#endif
