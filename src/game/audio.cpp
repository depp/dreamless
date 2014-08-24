/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "audio.hpp"
#include "sg/mixer.h"
#include "base/log.hpp"
#include "base/random.hpp"
#include <cstring>
namespace Game {

namespace {
struct AudioInfo {
    unsigned char offset;
    unsigned char count;
};

#include "audio_array.hpp"

sg_mixer_sound *audio_files[AUDIO_FILE_COUNT];
}

void Audio::play(unsigned time, Sfx sfx, float volume, float pan) {
    const AudioInfo &info = SFX_INFO[static_cast<int>(sfx)];
    int which = info.offset + Base::Random::gnexti(info.count);
    auto chan = sg_mixer_channel_play(
        audio_files[which], time, SG_MIXER_FLAG_DETACHED);
    sg_mixer_param param[2] = {
        { SG_MIXER_PARAM_VOL, volume },
        { SG_MIXER_PARAM_PAN, pan }
    };
    sg_mixer_channel_setparams(chan, param, 2);
}

void Audio::init() {
    sg_mixer_start();
    for (int i = 0; i < AUDIO_FILE_COUNT; i++) {
        char path[AUDIO_FILE_NAMELEN + 5];
        std::strcpy(path, "sfx/");
        std::strcat(path, AUDIO_FILE_NAMES[i]);
        audio_files[i] = sg_mixer_sound_file(
            path, std::strlen(path), nullptr);
        if (!audio_files[i])
            Base::Log::warn("%s: failed to load", path);
    }
}

}
