/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "game_screen.hpp"
#include "entity.hpp"
#include "item.hpp"
#include "minion.hpp"
#include "player.hpp"
#include <algorithm>
namespace Game {

static const int DREAM_TIME = 50;

static bool entity_is_alive(const std::unique_ptr<Entity> &p) {
    return p->team() != Team::DEAD;
}

GameScreen::GameScreen(const ControlState &ctl, const std::string &name)
    : Screen(ctl), m_drawn(false), m_dream(-1) {
    m_level.load(name);
    m_camera.set_bounds(m_level.bounds());
    m_camera.set_fov(IVec(Defs::WIDTH, Defs::HEIGHT));
    typedef Level::SpawnType Spawn;
    typedef Item::Type IType;
    for (auto &sp : m_level.spawn_points()) {
        switch (sp.type) {
        case Spawn::PLAYER:
            add_entity(new Player(*this, sp.pos));
            m_camera.set_target(FVec(sp.pos));
            m_camera.update();
            break;
        case Spawn::MINION:
            add_entity(new Minion(*this, sp.pos));
            break;
        case Spawn::DOOR_CLOSED:
            add_entity(new Item(*this, sp.pos, IType::DOOR_CLOSED));
            break;
        case Spawn::DOOR_LOCKED:
            add_entity(new Item(*this, sp.pos, IType::DOOR_LOCKED));
            break;
        case Spawn::KEY:
            add_entity(new Item(*this, sp.pos, IType::KEY));
            break;
        case Spawn::GATEWAY:
            add_entity(new Item(*this, sp.pos, IType::GATEWAY));
            break;
        }
    }
}

GameScreen::~GameScreen()
{ }

void GameScreen::draw(::Graphics::System &gr, int delta) {
    if (!m_drawn) {
        m_level.draw(gr);
        m_drawn = true;
    }

    float world;
    if (m_dream <= 0) {
        world = m_dream < 0 ? 1.0f : 0.0f;
    } else {
        world = (float) (m_dream * Defs::FRAMETIME - delta) *
            (float) (1.0f / (Defs::FRAMETIME * DREAM_TIME));
    }
    gr.set_world(world);

    for (auto &ent : m_entity)
        ent->draw(gr, delta);
    gr.set_camera(m_camera.drawpos(delta));
}

void GameScreen::update(unsigned time) {
    if (m_dream > 0)
        m_dream--;
    m_time = time;
    m_entity.insert(
        m_entity.end(),
        std::make_move_iterator(m_new_entity.begin()),
        std::make_move_iterator(m_new_entity.end()));
    m_new_entity.clear();
    for (auto &ent : m_entity)
        ent->update();
    auto part = std::stable_partition(
        m_entity.begin(), m_entity.end(), entity_is_alive);
    m_entity.erase(part, m_entity.end());
    m_camera.update();
}

void GameScreen::add_entity(Entity *ent) {
    if (ent)
        m_new_entity.push_back(std::unique_ptr<Entity>(ent));
}

void GameScreen::set_camera(FVec target) {
    m_camera.set_target(target);
}

void GameScreen::play_sound(Sfx sfx, float volume, FVec pos) {
    FVec delta = pos - m_camera.center();
    int width = Defs::WIDTH / 2;
    float panwidth = 0.8f;
    float pan = delta.x * (1.0f / (float) width * panwidth);
    if (pan > 1.0f)
        pan = 1.0f;
    else if (pan < -1.0f)
        pan = -1.0f;
    Audio::play(m_time + Defs::FRAMETIME, sfx, volume, pan);
}

void GameScreen::play_sound(Sfx sfx, float volume) {
    Audio::play(m_time + Defs::FRAMETIME, sfx, volume, 0.0f);
}

bool GameScreen::is_dreaming() const {
    return m_dream > DREAM_TIME / 2 || m_dream < 0;
}

void GameScreen::wake_up() {
    if (m_dream >= 0)
        return;
    m_dream = DREAM_TIME;
    play_sound(Sfx::WHA, -10.0f);
}

}
