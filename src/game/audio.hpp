/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_AUDIO_HPP
#define LD_GAME_AUDIO_HPP
#include "audio_enum.hpp"
namespace Game {

struct Audio {
    /// Play a sound effect.
    static void play(unsigned time, Sfx sfx, float volume, float pan);

    /// Load all sound effects.
    static void init();

    /// Start music, or adjust volume.
    static void music(unsigned time, float volume);
};

}
#endif
