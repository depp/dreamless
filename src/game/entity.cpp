/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "entity.hpp"
namespace Game {

Entity::Entity(GameScreen &scr, Team team)
    : m_screen(scr), m_id(scr.next_id()), m_team(team)
{ }

Entity::~Entity()
{ }

void Entity::update()
{ }

}
