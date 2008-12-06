#include "battlemints.h"
#include "controller.hpp"
#include "game.hpp"
#include "geom.hpp"
#include "intro.hpp"
#include "synth.hpp"
#include "goal.hpp"
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <boost/foreach.hpp>
#include <cstdlib>
#include <cstdio>
#include <limits>

using namespace battlemints;

extern "C"
void battlemints_start()
{
    sranddev();

    goal::global_start();        

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

    goal::global_finish();        
}
