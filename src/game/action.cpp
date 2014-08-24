/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "action.hpp"
namespace Game {
using Graphics::Sprite;

Sprite action_sprite(Action action) {
    static const Sprite ARRAY[ACTION_COUNT] = {
        Sprite::ACTION_JUMP,
        Sprite::ACTION_JUMPBACK,
        Sprite::ACTION_TURN,
        Sprite::ACTION_DROP
    };
    return ARRAY[static_cast<int>(action)];
}

}
