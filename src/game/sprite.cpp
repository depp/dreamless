/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sprite.hpp"
#include "base/sprite.hpp"
namespace Graphics {

const struct ::Base::Sprite SPRITES[] = {
#include "sprite_array.hpp"
    { nullptr, 0, 0, 0, 0, 0, 0 }
};

}
