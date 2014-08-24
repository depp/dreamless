/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_ITEM_HPP
#define LD_GAME_ITEM_HPP
#include "entity.hpp"
namespace Game {

class Item : public Entity {
public:
    enum class Type {
        DOOR_OPEN,
        DOOR_CLOSED,
        DOOR_LOCKED,
        KEY,
        ACTION_JUMP,
        ACTION_JUMPBACK,
        ACTION_TURN
    };

private:
    Type m_type;

public:
    Item(GameScreen &scr, IVec pos, Type type);
    virtual ~Item();

    virtual void draw(::Graphics::System &gr, int delta) const;
    Type type() const { return m_type; }
    void set_type(Type type) { m_type = type; }
    void destroy() { m_team = Team::DEAD; }

private:
    Sprite sprite() const;
};

}
#endif
