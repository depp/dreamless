/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "screen.hpp"
namespace Game {

Screen::Screen(const ControlState &ctl)
    : m_control(ctl)
{ }

Screen::~Screen()
{ }

void Screen::update(unsigned time) {
    (void) time;
}

}
