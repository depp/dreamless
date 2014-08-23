/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_PLAYER_HPP
#define LD_GAME_PLAYER_HPP
#include "entity.hpp"
namespace Game {

class Player : public Entity {
    FVec m_pos, m_pos0;

public:
    Player(GameScreen &scr, IVec pos);
    virtual ~Player();

    virtual void update(unsigned time);
    virtual void draw(::Graphics::System &gr, int delta) const;
};

}
#endif
