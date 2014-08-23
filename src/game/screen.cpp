/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "screen.hpp"
namespace Game {

Screen::Screen()
{ }

Screen::~Screen()
{ }

void Screen::update(unsigned time, const ControlState &ctl) {
    (void) time;
    (void) &ctl;
}

}
