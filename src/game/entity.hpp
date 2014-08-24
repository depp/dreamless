/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_ENTITY_HPP
#define LD_GAME_ENTITY_HPP
#include "base/vec.hpp"
// The following are not necessary, but useful.
#include "game_screen.hpp"
#include "control.hpp"
#include "defs.hpp"
namespace Game {

class GameScreen;

/// Teams for entities.
enum class Team {
    /// The entity is dead, and should be removed.
    DEAD,

    /// No special properties, such as decorative objects.
    AMBIENT,

    /// Player can interact with this object.  Causes contextual arrow.
    INTERACTIVE,

    /// The player, enemies will attack this.
    FRIEND,

    /// Enemies.
    FOE
};

class Entity {
protected:
    /// The enclosing game screen.
    GameScreen &m_screen;
    /// The unique identifier for this entity.
    int m_id;
    /// The team that this entity is on.
    Team m_team;
    /// The current position of this object.
    IVec m_pos;

public:
    Entity(GameScreen &sys, Team team);
    Entity(const Entity &) = delete;
    Entity(Entity &&) = delete;
    virtual ~Entity();
    Entity &operator=(const Entity &) = delete;
    Entity &operator=(Entity &&) = delete;

    /// Update the entity's state for the next frame.
    virtual void update();
    /// Draw the entity.
    virtual void draw(::Graphics::System &gr, int delta) const = 0;
    /// Get the entity's team.
    Team team() const { return m_team; }
    /// Get the entitiy's position.
    IVec pos() const { return m_pos; }
    /// Get the entity's unique ID number.
    int id() const { return m_id; }
};

}
#endif
