#include "sound_effects.hpp"
#include "synth.hpp"

namespace battlemints {

bool
impact_sound_effect::fill_buffer(int16_t *buffer, unsigned start, unsigned length)
{
    synth<
        sine_wave_synth<fraction<1,7> >,
        sine_wave_synth<fraction<1,7>, fraction<30,9> >,
        sine_wave_synth<fraction<9,70>, fraction<16,3> >,
        sine_wave_synth<fraction<8,70>, fraction<70,9> >,
        sine_wave_synth<fraction<6,70>, fraction<80,9> >,
        sine_wave_synth<fraction<5,70>, fraction<12,1> >,
        sine_wave_synth<fraction<5,70>, fraction<40,3> >,
        envelope<fraction<2,100>, fraction<3,100> >
    >(buffer, start, length, 1.0f, base_freq, 0.0f);

    return _buffer_n < 7;
}

}
