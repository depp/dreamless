/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_ACTION_HPP
#define LD_GAME_ACTION_HPP
#include "graphics/sprite.hpp"
namespace Game {

static const int ACTION_COUNT = 4;

enum class Action {
    JUMP,
    JUMP_BACK,
    TURN,
    DROP
};

Graphics::Sprite action_sprite(Action action);

}
#endif
