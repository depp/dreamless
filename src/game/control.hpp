/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_CONTROL_HPP
#define LD_GAME_CONTROL_HPP
namespace Game {

enum class Button {
    LEFT, RIGHT, UP, DOWN, SELECT, CANCEL, RESTART, ESCAPE
};

class ControlState {
private:
    const static int NBUTTONS = 8;
    int m_buttons[NBUTTONS];

    /// Read two button states and convert them to a continuous value.
    float button_to_axis(Button negative, Button positive) const;

public:
    ControlState();

    /// Clear the state of all keys.
    void clear();
    /// Update the state for the next frame.
    void update();
    /// Get the state for a key.
    bool get_button(Button k) const;
    /// Get whether the key was pressed this frame.
    bool get_button_instant(Button k) const;
    /// Get whether any key was pressed this frame.
    bool any_button_instant() const;
    /// Set the state for a key.
    void set_button(Button k, bool state);
    /// Get the x-axis state, in the range [-1,1].
    float get_xaxis() const;
    /// Get the y-axis state, in the range [-1,1].
    float get_yaxis() const;
};

}
#endif
