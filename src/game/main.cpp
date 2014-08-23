/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sg/entry.h"
#include "sg/event.h"
#include "sg/log.h"
#include "sg/mixer.h"
#include "base/sprite.hpp"
#include "graphics/system.hpp"
#include "graphics/sprite.hpp"
#include "control.hpp"
#include "defs.hpp"
#include "game_screen.hpp"
#include "screen.hpp"

namespace Game {
class Main {
private:
    sg_logger *m_logger;
    ControlState m_control;
    std::unique_ptr<Graphics::System> m_graphics;
    std::unique_ptr<Screen> m_screen;
    bool m_initted;
    unsigned m_frametime;

public:
    static Main *main;

    Main() :m_logger(sg_logger_get("game")), m_initted(false) {
        m_screen.reset(new GameScreen(m_control, "01"));
    }

    void event(union sg_event *evt) {
        switch (evt->type) {
        case SG_EVENT_VIDEO_INIT:
            if (!m_graphics)
                m_graphics.reset(new Graphics::System);
            break;

        default:
            break;
        }
    }

    void draw(int width, int height, unsigned msec) {
        advance(msec);

        Graphics::System &gr = *m_graphics;
        int delta = msec - m_frametime;
        gr.begin(width, height);
        m_screen->draw(gr, delta);
        gr.end();
        gr.draw();
    }

private:
    void advance(unsigned time) {
        unsigned nframes;
        if (m_initted) {
            unsigned delta = time - m_frametime;
            if (delta > Defs::MAXUPDATE) {
                sg_logs(m_logger, SG_LOG_WARN, "Lag");
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

        for (unsigned i = 0; i < nframes; i++) {
            unsigned utime =
                m_frametime + (i - nframes + 1) * Defs::FRAMETIME;
            sg_mixer_settime(utime);
            m_screen->update(utime);
            sg_mixer_commit();
            m_control.update();
        }
    }
};

Main *Main::main;
}

void sg_game_init(void) {
    Game::Main::main = new Game::Main;
}

void sg_game_destroy(void) {
    delete Game::Main::main;
}

void sg_game_getinfo(struct sg_game_info *info) {
    info->name = "Dreamless";
}

void sg_game_event(union sg_event *evt) {
    Game::Main::main->event(evt);
}

void sg_game_draw(int width, int height, unsigned msec) {
    Game::Main::main->draw(width, height, msec);
}
