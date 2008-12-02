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

namespace battlemints {

const float BORDER_THICKNESS = 0.045f;
const float PIXELS_PER_GAME_UNIT = 30.0f;
const float GAME_UNITS_PER_PIXEL = 1.0f / PIXELS_PER_GAME_UNIT;
const float PLAYER_ACCEL_SCALE = 0.02f;
const float FRICTION = 0.93f;
const float MOVEMENT_THRESHOLD = 0.000001f;
const vec2 ZERO_VEC2 = make_vec2(0.0);
const vec2 GAME_WINDOW_SIZE = make_vec2(320.0, 480.0);
const vec2 GAME_WINDOW_UNIT_SIZE = make_vec2(320.0, 480.0) * GAME_UNITS_PER_PIXEL;

vec2 controller_state;

}

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
