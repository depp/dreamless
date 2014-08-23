/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sg/entry.h"
#include "sg/opengl.h"
#include "sg/mixer.h"

void
sg_game_init(void)
{
}

void
sg_game_destroy(void)
{ }

void
sg_game_getinfo(struct sg_game_info *info)
{
    info->name = "Dreamless";
}

void
sg_game_event(union sg_event *evt)
{
    (void) evt;
}

void
sg_game_draw(int width, int height, unsigned msec)
{
    sg_mixer_settime(msec);

    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    sg_mixer_commit();
}
