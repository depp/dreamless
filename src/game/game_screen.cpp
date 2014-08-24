/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "game_screen.hpp"
#include "entity.hpp"
#include "item.hpp"
#include "minion.hpp"
#include "player.hpp"
#include "main.hpp"
#include "base/random.hpp"
#include <algorithm>
namespace Game {

static const int DREAM_TIME = 50;
static const int WIN_DELAY = 50;
static const float NOISE_SPEED = 0.01f;

static bool entity_is_alive(const std::unique_ptr<Entity> &p) {
    return p->team() != Team::DEAD;
}

GameScreen::GameScreen(const ControlState &ctl, int levelnum)
    : Screen(ctl), m_levelnum(levelnum), m_drawn(false), m_dream(-1),
      m_minions(0), m_wincounter(-1) {
    m_level.load(std::to_string(levelnum));
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
            m_minions++;
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
    for (int i = 0; i < 2; i++) {
        float angle = std::atan(1.0f) * 8.0 * Base::Random::gnextf();
        m_noise[2*i+0] = 0.0f;
        m_noise[2*i+1] = 0.0f;
        m_noisevel[2*i+0] = std::cos(angle) * NOISE_SPEED;
        m_noisevel[2*i+1] = std::sin(angle) * NOISE_SPEED;
    }

    /// End condition (the last level)
    if (!m_minions)
        m_minions = -1;
}

GameScreen::~GameScreen()
{ }

void GameScreen::draw(::Graphics::System &gr, int delta) {
    if (!m_drawn) {
        gr.clear(true);
        m_level.draw(gr);
        m_drawn = true;
    } else {
        gr.clear(false);
    }

    float noise[4];
    for (int i = 0; i < 4; i++) {
        noise[i] = m_noise[i] +
            m_noisevel[i] * (delta * (1.0f / Defs::FRAMETIME));
    }
    gr.set_noise(noise);

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
    if (m_wincounter > 0) {
        if (!--m_wincounter)
            Main::main->load_level(m_levelnum + 1);
    }

    for (int i = 0; i < 4; i++) {
        m_noise[i] += m_noisevel[i];
        if (m_noise[i] > +1.0f)
            m_noise[i] -= 1.0f;
        else if (m_noise[i] < -1.0f)
            m_noise[i] += 1.0f;
    }

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

void GameScreen::capture_minion() {
    m_minions--;
    if (m_minions)
        return;
    play_sound(Sfx::DREAM, -3.0f);
    m_wincounter = WIN_DELAY;
}

}
