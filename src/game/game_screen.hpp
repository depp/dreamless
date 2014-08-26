/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_GAME_SCREEN_HPP
#define LD_GAME_GAME_SCREEN_HPP
#include "level.hpp"
#include "screen.hpp"
#include "camera.hpp"
#include "audio.hpp"
#include "analytics/analytics.hpp"
#include <memory>
#include <string>
#include <vector>
namespace Game {
class Entity;

class GameScreen : public Screen {
    /// The level number.
    int m_levelnum;
    /// Whether static data has been uploaded.
    bool m_drawn;
    /// The current level data.
    Level m_level;
    /// The level camera.
    Camera m_camera;
    /// List of active entities.
    std::vector<std::unique_ptr<Entity>> m_entity;
    /// List of new entities, not yet active.
    std::vector<std::unique_ptr<Entity>> m_new_entity;
    /// Current timestamp.
    unsigned m_time;
    /// If -1, we are dreaming.  0 is awake.  Positive is countdown to
    /// wake up.
    int m_dream;
    /// Noise to feed to the graphics system.
    float m_noise[4];
    float m_noisevel[4];
    /// Minions remaining to clear the level.
    int m_minions;
    int m_wincounter;
    /// Unique ID number generator.
    int m_id;
    /// Analytics info.
    Analytics::Level m_analytics;

public:
    GameScreen(const ControlState &ctl, int levelnum, unsigned time);
    virtual ~GameScreen();

    /// Draw the screen.
    virtual void draw(::Graphics::System &gr, int delta);
    /// Update the screen for the next frame.
    virtual void update(unsigned time);

    /// Add an entity to the level, takes ownership.  NULL is ok.
    void add_entity(Entity *ent);

    /// Accessor for the level.
    const Level &level() const { return m_level; }

    /// Set the camera target.
    void set_camera(FVec target, bool override=false);

    /// Get the list of all entities.
    const std::vector<std::unique_ptr<Entity>> &entities() const {
        return m_entity;
    }

    /// Play a sound at the given location.
    void play_sound(Sfx sfx, float volume, FVec pos);

    /// Play a sound with no associated location.
    void play_sound(Sfx sfx, float volume);

    /// Whether we are currently in dream.
    bool is_dreaming() const;

    /// Wake up from the dream.
    void wake_up();

    /// Register a capture.  When all minions are captured, the level
    /// is won.
    void capture_minion();

    /// Generate a unique ID number.
    int next_id() {
        return m_id++;
    }

    /// Get the analytics data.
    Analytics::Level &analytics() {
        return m_analytics;
    }
};

}
#endif
