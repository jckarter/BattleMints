#include "battlemints.h"
#include "controller.hpp"
#include "game.hpp"
#include "geom.hpp"
#include "intro.hpp"
#include "synth.hpp"
#include "goal.hpp"
#include "dramatis_personae.hpp"
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <boost/foreach.hpp>
#include <cstdlib>
#include <cstdio>

using namespace battlemints;

extern "C"
void battlemints_start()
{
    sranddev();

    player::global_start();        
    goal::global_start();        
    mini::global_start();
    mega::global_start();

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

    mega::global_finish();
    mini::global_finish();
    goal::global_finish();        
    player::global_finish();
}
