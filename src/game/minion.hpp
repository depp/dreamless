/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_MINION_HPP
#define LD_GAME_MINION_HPP
#include "entity.hpp"
#include "physics.hpp"
namespace Game {
class Item;

class Minion : public Entity {
    static const Walker::Stats STATS;

    Mover m_mover;
    Walker m_walker;
    int m_direction;

public:
    Minion(GameScreen &scr, IVec pos);
    virtual ~Minion();

    virtual void update(unsigned time);
    virtual void draw(::Graphics::System &gr, int delta) const;

private:
    void hit_item(Item &item);
};

}
#endif
