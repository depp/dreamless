/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "control.hpp"
#include "sg/entry.h"
namespace Game {

ControlState::ControlState() {
    clear();
}

float ControlState::button_to_axis(Button negative, Button positive) const {
    bool nstate = m_buttons[static_cast<int>(negative)] != 0;
    bool pstate = m_buttons[static_cast<int>(positive)] != 0;
    return 0.0f + (nstate ? -1.0f : 0.0f) + (pstate ? +1.0f : 0.0f);
}

void ControlState::clear() {
    for (int i = 0; i < NBUTTONS; i++)
        m_buttons[i] = 0;
}

void ControlState::update() {
    for (int i = 0; i < NBUTTONS; i++) {
        if (m_buttons[i] == 1)
            m_buttons[i] = 2;
    }
}

bool ControlState::get_button(Button k) const {
    int index = static_cast<int>(k);
    if (index < 0 || index >= NBUTTONS)
        sg_sys_abort("unknown button");
    return m_buttons[index] != 0;
}

bool ControlState::get_button_instant(Button k) const {
    int index = static_cast<int>(k);
    if (index < 0 || index >= NBUTTONS)
        sg_sys_abort("unknown button");
    return m_buttons[index] == 1;
}

bool ControlState::any_button_instant() const {
    for (int i = 0; i < NBUTTONS; i++)
        if (m_buttons[i] == 1)
            return true;
    return false;
}

void ControlState::set_button(Button k, bool state) {
    int index = static_cast<int>(k);
    if (index < 0 || index >= NBUTTONS)
        sg_sys_abort("unknown button");
    if (state) {
        if (m_buttons[index] == 0)
            m_buttons[index] = 1;
    } else {
        m_buttons[index] = 0;
    }
}

Base::FVec ControlState::get_2d() const {
    return Base::FVec(
        button_to_axis(Button::LEFT, Button::RIGHT),
        button_to_axis(Button::DOWN, Button::UP));
}

}
