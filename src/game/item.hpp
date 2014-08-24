/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_ITEM_HPP
#define LD_GAME_ITEM_HPP
#include "entity.hpp"
#include "action.hpp"
namespace Game {

class Item : public Entity {
public:
    enum class Type {
        DOOR_OPEN,
        DOOR_CLOSED,
        DOOR_LOCKED,
        KEY,
        ACTION
    };

private:
    Type m_type;
    Action m_action;

public:
    Item(GameScreen &scr, IVec pos, Type type);
    virtual ~Item();

    virtual void draw(::Graphics::System &gr, int delta) const;
    Type type() const { return m_type; }
    Action action() const { return m_action; }
    void set_type(Type type) { m_type = type; }
    void set_action(Action action) { m_action = action; }
    void destroy() { m_team = Team::DEAD; }

private:
    Sprite sprite() const;
};

}
#endif
