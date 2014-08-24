/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_CAMERA_HPP
#define LD_GAME_CAMERA_HPP
#include "defs.hpp"
#include <vector>
namespace Game {

class Camera {
    IRect m_bounds;
    IRect m_camera;
    FRect m_clamp;
    std::vector<FVec> m_history;
    FVec m_target, m_pos0, m_pos1;

public:
    Camera();

    /// Set the camera bounds.  The entire camera (not just the
    /// center) will stay inside the bounds, if possible.
    void set_bounds(IRect bounds);

    /// Set the camera field of view.  Must be called at least once.
    void set_fov(IVec camera_size);

    /// Update the camera position.
    void update();

    /// Set the target for the camera center.
    void set_target(FVec target);

    /// Get the lower-left origin of the camera.
    IVec drawpos(int delta) const;

    /// Get the current center.
    FVec center() const;

private:
    void calculate_clamp();
};

}
#endif
