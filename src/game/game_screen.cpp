/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "game_screen.hpp"
#include "entity.hpp"
#include "player.hpp"
#include <algorithm>
namespace Game {

static bool entity_is_alive(const std::unique_ptr<Entity> &p) {
    return p->m_team != Team::DEAD;
}

GameScreen::GameScreen(const ControlState &ctl, const std::string &name)
    : Screen(ctl) {
    m_level.load(name);
    add_entity(new Player(*this, IVec(0, 16)));
}

GameScreen::~GameScreen()
{ }

void GameScreen::draw(::Graphics::System &gr, int delta) {
    for (auto &ent : m_entity)
        ent->draw(gr, delta);
}

void GameScreen::update(unsigned time) {
    m_entity.insert(
        m_entity.end(),
        std::make_move_iterator(m_new_entity.begin()),
        std::make_move_iterator(m_new_entity.end()));
    m_new_entity.clear();
    for (auto &ent : m_entity)
        ent->update(time);
    auto part = std::stable_partition(
        m_entity.begin(), m_entity.end(), entity_is_alive);
    m_entity.erase(part, m_entity.end());
}

void GameScreen::add_entity(Entity *ent) {
    if (ent)
        m_new_entity.push_back(std::unique_ptr<Entity>(ent));
}

}
