/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_MAIN_HPP
#define LD_GAME_MAIN_HPP
#include "control.hpp"
#include <memory>
union sg_event;
namespace Graphics {
class System;
}
namespace Game {
class Screen;

class Main {
private:
    ControlState m_control;
    std::unique_ptr<Graphics::System> m_graphics;
    std::unique_ptr<Screen> m_screen;
    bool m_initted;
    unsigned m_frametime;
    int m_pending;

public:
    static Main *main;

    Main();
    ~Main();
    void event(sg_event &evt);
    void draw(int width, int height, unsigned msec);
    void load_level(int level);

private:
    void event_key(int key, bool state);
    void advance(unsigned time);
};

}
#endif
