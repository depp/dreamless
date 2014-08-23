/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_ENTITY_HPP
#define LD_GAME_ENTITY_HPP
#include "base/vec.hpp"
// The following are not necessary, but useful.
#include "base/sprite.hpp"
#include "game_screen.hpp"
#include "control.hpp"
#include "defs.hpp"
#include "graphics/layer.hpp"
#include "graphics/sprite.hpp"
#include "graphics/system.hpp"
namespace Game {

// Re-export common types.
typedef ::Base::Orientation Orientation;
typedef ::Base::IVec IVec;
typedef ::Base::IRect IRect;
typedef ::Base::FVec FVec;
typedef ::Base::FRect FRect;
typedef ::Graphics::AnySprite AnySprite;
typedef ::Graphics::Sprite Sprite;
typedef ::Graphics::Tile Tile;
typedef ::Graphics::Layer Layer;

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
public:
    /// The enclosing game screen.
    GameScreen &m_screen;
    /// The team that this entity is on.
    Team m_team;
    /// Bounding box.
    IRect m_bbox;

    Entity(GameScreen &sys, Team team);
    Entity(const Entity &) = delete;
    Entity(Entity &&) = delete;
    virtual ~Entity();
    Entity &operator=(const Entity &) = delete;
    Entity &operator=(Entity &&) = delete;

    /// Update the entity's state for the next frame.
    virtual void update(unsigned time);
    /// Draw the entity.
    virtual void draw(::Graphics::System &gr, int delta) = 0;
};

}
#endif
