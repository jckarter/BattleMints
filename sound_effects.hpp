#ifndef __SOUND_EFFECTS_HPP_
#define __SOUND_EFFECTS_HPP_

#include "sound_server.hpp"

namespace battlemints {

struct impact_sound_effect : sound_effect {
    impact_sound_effect(float bf) : sound_effect(2, 1024), base_freq(bf) {}

    virtual bool fill_buffer(int16_t *buffer, unsigned start, unsigned length);

    float base_freq;
};

}

#endif
