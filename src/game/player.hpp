/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_PLAYER_HPP
#define LD_GAME_PLAYER_HPP
#include "entity.hpp"
#include "physics.hpp"
namespace Game {
class Item;

class Player : public Entity {
    static const Walker::Stats STATS_PHYSICAL;
    static const Walker::Stats STATS_DREAM;

    Mover m_mover;
    Walker m_walker;
    int m_selection;
    int m_num_actions;
    Action m_actions[ACTION_COUNT];
    int m_direction;
    int m_dialogue;
    int m_dialogue_time;

public:
    Player(GameScreen &scr, IVec pos);
    virtual ~Player();

    virtual void update();
    virtual void draw(::Graphics::System &gr, int delta) const;

private:
    void hit_item(Item &item);
    void show_dialogue(int index);
};

}
#endif
