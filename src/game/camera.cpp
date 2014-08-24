/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "camera.hpp"
namespace Game {

Camera::Camera()
    : m_bounds(IRect::zero()), m_camera(IRect::zero()),
      m_clamp(FRect::zero()), m_target(FVec::zero()),
      m_pos0(FVec::zero()), m_pos1(FVec::zero())
{ }

void Camera::set_bounds(IRect bounds) {
    m_bounds = bounds;
    calculate_clamp();
}

void Camera::set_fov(IVec camera_size) {
    m_camera.x0 = - (camera_size.x >> 1);
    m_camera.x1 = m_camera.x0 + camera_size.x;
    m_camera.y0 = - (camera_size.y >> 1);
    m_camera.y1 = m_camera.y0 + camera_size.y;
    calculate_clamp();
}

void Camera::update() {
    static const int MAX_HISTORY = 32;
    if (m_history.size() != MAX_HISTORY) {
        m_history.resize(MAX_HISTORY, m_target);
        m_pos0 = m_pos1 = m_target;
        return;
    }
    m_history.pop_back();
    m_history.insert(m_history.begin(), m_target);

    // Triangular filter
    double accx = 0.0, accy = 0.0, accw = 0.0;
    for (int i = 0; i < MAX_HISTORY; i++) {
        int w1 = i + 1;
        int w2 = MAX_HISTORY - i;
        double weight = w1 > w2 ? w1 : w2;
        accx += weight * m_history[i].x;
        accy += weight * m_history[i].y;
        accw += weight;
    }
    m_pos0 = m_pos1;
    m_pos1.x = (float) (accx / accw);
    m_pos1.y = (float) (accy / accw);
}

void Camera::set_target(FVec target) {
    m_target = m_clamp.nearest(target);
}

IVec Camera::drawpos(int delta) const {
    return Defs::interp(m_pos0, m_pos1, delta) +
        IVec(m_camera.x0, m_camera.y0);
}

FVec Camera::center() const {
    return m_pos1;
}

void Camera::calculate_clamp() {
    if (m_bounds.width() <= m_camera.width()) {
        m_clamp.x0 = m_clamp.x1 = (m_bounds.x0 + m_bounds.x1) >> 1;
    } else {
        m_clamp.x0 = m_bounds.x0 - m_camera.x0;
        m_clamp.x1 = m_bounds.x1 - m_camera.x1;
    }
    if (m_bounds.height() <= m_camera.height()) {
        m_clamp.y0 = m_clamp.y1 = (m_bounds.y0 + m_bounds.y1) >> 1;
    } else {
        m_clamp.y0 = m_bounds.y0 - m_camera.y0;
        m_clamp.y1 = m_bounds.y1 - m_camera.y1;
    }
}

}
