/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sg/entry.h"
#include "sg/event.h"
#include "sg/mixer.h"
#include "base/sprite.hpp"
#include "graphics/system.hpp"
#include "graphics/sprite.hpp"

static ::Graphics::System *graphics;

void sg_game_init(void) {
}

void sg_game_destroy(void) {
    delete graphics;
}

void sg_game_getinfo(struct sg_game_info *info) {
    info->name = "Dreamless";
}

void sg_game_event(union sg_event *evt) {
    switch (evt->type) {
    case SG_EVENT_VIDEO_INIT:
        graphics = new Graphics::System;
        break;

    default:
        break;
    }
}

void sg_game_draw(int width, int height, unsigned msec) {
    sg_mixer_settime(msec);

    Graphics::System &s = *graphics;
    s.begin(width, height);
    s.add_sprite(
        Graphics::Sprite::KNIGHT_1, 100, 100, Base::Orientation::NORMAL);
    s.end();
    s.draw();

    sg_mixer_commit();
}
