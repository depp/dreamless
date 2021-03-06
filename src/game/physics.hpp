/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_PHYSICS_HPP
#define LD_GAME_PHYSICS_HPP
#include "defs.hpp"
namespace Game {
class Level;

/// Utility class for objects that move.
class Mover {
    FVec m_pos0, m_pos1;

public:
    Mover(IVec pos) : m_pos0(pos), m_pos1(pos) { }

    /// Update the mover, setting the new location.
    void update(FVec new_pos) {
        m_pos0 = m_pos1;
        m_pos1 = new_pos;
    }

    /// Get the current location.
    FVec pos() const {
        return m_pos1;
    }

    /// Get the previous location.
    FVec lastpos() const {
        return m_pos0;
    }

    /// Get the draw location.
    IVec drawpos(int delta) const {
        return Defs::interp(m_pos0, m_pos1, delta);
    }
};

/// Utility class for objects that can walk and jump.
class Walker {
private:
    enum class State { WALK, AIR, DOUBLE };

    /// The state of the jump, or lack thereof.
    State m_state;
    /// Time remaining with jump control, or -1 if jump was released.
    int m_jumptime;
    /// The distance traveled since the last footstep.
    float m_stepdistance;

public:
    struct Stats {
        // Horizontal movement
        float accel_ground;
        float speed_ground;
        float accel_air;
        float speed_air;

        // Jumping
        int jump_time;
        float jump_accel;
        float jump_speed;
        float jump_gravity;
        bool jump_double;

        // Time interval between footsteps
        float step_time;
    };

public:
    /// Horizontal movement blocked by obstacle.
    static const unsigned FLAG_BLOCKED = 1u << 0;
    /// Did jump.
    static const unsigned FLAG_JUMPED = 1u << 1;
    /// The jump is a double jump.
    static const unsigned FLAG_DOUBLE = 1u << 2;
    /// Is currently airborne.
    static const unsigned FLAG_AIRBORNE = 1u << 3;
    /// Should play a footstep sound.
    static const unsigned FLAG_FOOTSTEP = 1u << 4;

    Walker();

    /// This will also update the mover.  Returns flags.
    unsigned update(const struct Stats &stats, const Level &level,
                    Mover &mover, FVec drive);
};

}
#endif
