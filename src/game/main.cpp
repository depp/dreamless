/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "main.hpp"

#include "sg/entry.h"
#include "sg/event.h"
#include "sg/mixer.h"
#include "sg/record.h"
#include "sg/keycode.h"
#include "base/sprite.hpp"
#include "graphics/system.hpp"
#include "graphics/sprite.hpp"
#include "control.hpp"
#include "defs.hpp"
#include "game_screen.hpp"
#include "screen.hpp"
#include "audio.hpp"
#include "analytics/analytics.hpp"

namespace {
const float MUSIC_VOLUME = -12.0;
}

namespace Game {

Main::Main() : m_initted(false), m_pending(1) { }

Main::~Main() {
    // The destructor is not really safe, it calls OpenGL functions.
    // So we intentionally leak the object.
    m_graphics.release();
}

void Main::event(sg_event &evt) {
    switch (evt.type) {
    case SG_EVENT_VIDEO_INIT:
        if (!m_graphics)
            m_graphics.reset(new Graphics::System);
        break;

    case SG_EVENT_KDOWN:
        event_key(evt.key.key, true);
        break;

    case SG_EVENT_KUP:
        event_key(evt.key.key, false);
        break;

    default:
        break;
    }
}

void Main::draw(int width, int height, unsigned msec) {
    advance(msec);

    Graphics::System &gr = *m_graphics;
    int delta = msec - m_frametime;
    gr.set_size(width, height);
    m_screen->draw(gr, delta);
    gr.finalize();
    gr.draw();
}

void Main::load_level(int level) {
    m_pending = level;
}

void Main::event_key(int key, bool state) {
    Button button;
    switch (key) {
    case KEY_A:
    case KEY_Left:
        button = Button::LEFT;
        break;

    case KEY_D:
    case KEY_Right:
        button = Button::RIGHT;
        break;

    case KEY_W:
    case KEY_Up:
        button = Button::UP;
        break;

    case KEY_S:
    case KEY_Down:
        button = Button::DOWN;
        break;

    case KEY_E:
    case KEY_Tab:
        button = Button::NEXT;
        break;

    case KEY_Q:
        button = Button::PREV;
        break;

    case KEY_Space:
        button = Button::ACTION;
        break;

    case KEY_R:
    case KEY_F5:
    case KEY_Home:
        button = Button::RESTART;
        break;

    case KEY_Escape:
        button = Button::ESCAPE;
        break;

    case KEY_F1:
        button = Button::HELP;
        break;

    case KEY_F7:
    case KEY_PageUp:
        button = Button::PREVLEVEL;
        break;

    case KEY_F8:
    case KEY_PageDown:
        button = Button::NEXTLEVEL;
        break;

    case KEY_F10:
        if (state)
            sg_record_start(m_frametime + Defs::FRAMETIME);
        return;

    case KEY_F11:
        if (state)
            sg_record_stop();
        return;

    case KEY_F12:
        if (state)
            sg_record_screenshot();
        return;

    default:
        return;
    }
    m_control.set_button(button, state);
}

void Main::advance(unsigned time) {
    unsigned nframes;
    if (m_initted) {
        Audio::music(time, MUSIC_VOLUME);
        unsigned delta = time - m_frametime;
        if (delta > Defs::MAXUPDATE) {
            Log::warn("lag");
            nframes = 1;
            m_frametime = time;
        } else {
            nframes = delta / Defs::FRAMETIME;
            m_frametime += nframes * Defs::FRAMETIME;
        }
    } else {
        m_initted = true;
        m_frametime = time;
        nframes = 1;
    }

    if (m_pending) {
        m_screen.reset(new GameScreen(m_control, m_pending, time));
        m_pending = 0;
        nframes = 1;
    }

    for (unsigned i = 0; i < nframes; i++) {
        unsigned utime =
            m_frametime + (i - nframes + 1) * Defs::FRAMETIME;
        sg_mixer_settime(utime);
        m_screen->update(utime);
        sg_mixer_commit();
        m_control.update();
    }
}

Main *Main::main;

}

void sg_game_init(void) {
    Base::Log::init();
    Game::Audio::init();
    Analytics::Analytics::init();
    Game::Main::main = new Game::Main;
}

void sg_game_destroy(void) {
    delete Game::Main::main;
    Analytics::Analytics::finish();
}

void sg_game_getinfo(struct sg_game_info *info) {
    info->name = "Dreamless";
}

void sg_game_event(union sg_event *evt) {
    Game::Main::main->event(*evt);
}

void sg_game_draw(int width, int height, unsigned msec) {
    Game::Main::main->draw(width, height, msec);
}
