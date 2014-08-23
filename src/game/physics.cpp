/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "physics.hpp"
#include "level.hpp"
namespace Game {

Walker::Walker()
    : m_state(State::AIR), m_jumptime(-1)
{ }

void Walker::update(const struct Stats &stats, const Level &level,
                    Mover &mover, FVec drive) {
    const int STEPS = 8;
    FVec pos = mover.pos(), accel(FVec::zero());
    FVec vel = (pos - mover.lastpos()) * Defs::invdt();

    // Calculate horizontal acceleration.
    float max_speed, max_accel;
    if (m_state == State::WALK) {
        max_speed = stats.speed_ground;
        max_accel = stats.accel_ground;
    } else {
        max_speed = stats.speed_air;
        max_accel = stats.accel_air;
    }
    accel.x = (max_speed * drive.x - vel.x) * Defs::invdt();
    if (accel.x > max_accel)
        accel.x = max_accel;
    else if (accel.x < -max_accel)
        accel.x = -max_accel;

    // Calculate vertical acceleration.
    if (drive.y >= 0.5f) {
        bool can_jump = m_jumptime < 0 &&
            (m_state == State::WALK ||
             (m_state == State::AIR && stats.jump_double));
        if (m_jumptime > 0) {
            m_jumptime--;
            accel.y += stats.jump_accel * drive.y;
        } else if (can_jump) {
            if (m_state == State::WALK)
                m_state = State::AIR;
            else
                m_state = State::DOUBLE;
            m_jumptime = stats.jump_time;
            float dv = stats.jump_speed - vel.y;
            if (dv > 0.0f)
                accel.y += dv * Defs::invdt();
        }
    } else {
        m_jumptime = -1;
    }
    if (m_state != State::WALK) {
        accel.y += -stats.jump_gravity;
    }

    // Update velocity and position
    vel += accel * Defs::dt();
    FVec newpos = pos + vel * Defs::dt();

    // Handle horizontal collisions
    if (vel.x != 0.0f) {
        FVec test1(8.0f, -10.0f), test2(8.0f, 10.0f);
        if (vel.x < 0) {
            test1.x = -test1.x;
            test2.x = -test2.x;
        }
        int scale = STEPS;
        while (level.hit_test(newpos + test1) ||
               level.hit_test(newpos + test2)) {
            scale--;
            if (!scale) {
                newpos.x = pos.x;
                break;
            }
            float frac = (float) scale * (1.0f / (float) STEPS);
            newpos.x = pos.x + vel.x * (frac * Defs::dt());
        }
    }

    // Handle collisions with the ceiling
    if (vel.y > 0.0f) {
        FVec head(0.0f, 14.0f);
        int scale = STEPS;
        while (level.hit_test(newpos + head)) {
            scale--;
            if (!scale) {
                newpos.y = pos.y;
                break;
            }
            float frac = (float) scale * (1.0f / (float) STEPS);
            newpos.y = pos.y + vel.y * (frac * Defs::dt());
        }
    }

    // Handle collisions with the floor
    FVec walkpos(0.0f, -14.0f);
    float floor = level.find_floor(newpos + walkpos) - walkpos.y;
    if (m_state == State::WALK) {
        float delta = floor - pos.y;
        float slope = delta / (newpos.x - pos.x);
        if (std::abs(slope) < 1.1f || std::abs(delta) < 4.0f) {
            newpos.y = floor;
        } else if (delta < 0) {
            m_state = State::AIR;
        } else {
            newpos = pos;
        }
    } else {
        if (newpos.y <= floor) {
            m_jumptime = 0;
            m_state = State::WALK;
            newpos.y = floor;
        }
    }

    mover.update(newpos);
}

}
