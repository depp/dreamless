/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_MINION_HPP
#define LD_GAME_MINION_HPP
#include "entity.hpp"
#include "physics.hpp"
namespace Game {
class Item;
enum class Action;

class Minion : public Entity {
    static const Walker::Stats STATS;

    enum State {
        WALK,
        JUMP
    };

    Mover m_mover;
    Walker m_walker;
    State m_state;
    int m_statetime;
    int m_direction;
    bool m_haskey;

public:
    Minion(GameScreen &scr, IVec pos);
    virtual ~Minion();

    virtual void update();
    virtual void draw(::Graphics::System &gr, int delta) const;

private:
    void hit_item(Item &item);
    void do_action(Action action);
};

}
#endif
