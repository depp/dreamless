/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "physics.hpp"
namespace Game {

Walker::Walker()
    : m_airborne(true), m_velocity(FVec::zero())
{ }

void Walker::update(const struct Stats &stats,
                    Mover &mover, FVec drive) {
    FVec pos = mover.pos(), vel = m_velocity;

    // Horizontal speed and acceleration.
    float max_speed = stats.speed_ground;
    float max_accel = stats.accel_ground;

    // Choose an acceleration that will take us to our target velocity
    // in one frame.
    float accel = (max_speed * drive.x - vel.x) * Defs::invdt();

    if (accel > max_accel)
        accel = max_accel;
    else if (accel < -max_accel)
        accel = -max_accel;
    vel.x += accel * Defs::dt();

    pos += vel * Defs::dt();

    m_velocity = vel;
    mover.update(pos);
}

}
