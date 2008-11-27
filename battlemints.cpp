#include "battlemints.h"
#include "controller.hpp"
#include "game.hpp"
#include "geom.hpp"
#include "intro.hpp"
#include "synth.hpp"
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <boost/foreach.hpp>
#include <cstdlib>
#include <cstdio>

namespace battlemints {

vec2 controller_state;
sound_server *sounds;
sound_effect *impact_sound;

}

using namespace battlemints;

extern "C"
void battlemints_start()
{
    sranddev();

    sounds = new sound_server(16);
    
    int16_t buffer_data[7000];
    synth<
        sine_wave_synth<fraction<1,7> >,
        sine_wave_synth<fraction<1,7>, fraction<30,9> >,
        sine_wave_synth<fraction<9,70>, fraction<16,3> >,
        sine_wave_synth<fraction<8,70>, fraction<70,9> >,
        sine_wave_synth<fraction<6,70>, fraction<80,9> >,
        sine_wave_synth<fraction<5,70>, fraction<12,1> >,
        sine_wave_synth<fraction<5,70>, fraction<40,3> >,
        envelope<fraction<2,100>, fraction<3,100> >
    >(buffer_data, 7000, 1.0f, 450.0f, 0.0f);
    impact_sound = new sound_effect(buffer_data, 7000);

    controller::set_current(new intro());
}

extern "C"
void battlemints_tick()
{
    controller::tick_current();
}

extern "C"
void battlemints_draw()
{
    controller::draw_current();
}

extern "C"
void battlemints_input(int player, float x, float y, int buttons)
{
    controller_state = make_vec2(x, y);
}

extern "C"
void battlemints_finish()
{
    controller::delete_current();

    delete impact_sound;
    delete sounds;
}
