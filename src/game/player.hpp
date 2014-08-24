/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_PLAYER_HPP
#define LD_GAME_PLAYER_HPP
#include "entity.hpp"
#include "physics.hpp"
namespace Game {

class Player : public Entity {
    static const Walker::Stats STATS;

    Mover m_mover;
    Walker m_walker;

public:
    Player(GameScreen &scr, IVec pos);
    virtual ~Player();

    virtual void update();
    virtual void draw(::Graphics::System &gr, int delta) const;
};

}
#endif
