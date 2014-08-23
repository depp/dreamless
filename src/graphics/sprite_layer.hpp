/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SPRITE_LAYER_HPP
#define LD_GRAPHICS_SPRITE_LAYER_HPP
#include "base/sprite.hpp"
#include "base/vec.hpp"
#include "layer.hpp"
#include "sprite.hpp"
namespace Graphics {
class CommonData;

class SpriteLayer {
    Base::SpriteSheet m_sheet;
    Base::SpriteArray m_array[NLAYER];

public:
    SpriteLayer();

    void clear(bool all);
    void add_sprite(AnySprite sp, Base::IVec pos,
                    Base::Orientation orientation,
                    Layer layer);
    void upload();
    void draw(const CommonData &com);

private:
    Base::SpriteArray &array(Layer layer) {
        return m_array[static_cast<int>(layer)];
    }

    void draw_layer(const CommonData &com, Layer layer);
};

}
#endif
