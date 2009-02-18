#include "battlemints.h"
#include "controller.hpp"
#include "drawing.hpp"
#include "game.hpp"
#include "geom.hpp"
#include "intro.hpp"
#include "synth.hpp"
#include "goal.hpp"
#include "dramatis_personae.hpp"
#include "renderers.hpp"
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <boost/foreach.hpp>
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

using namespace battlemints;

extern "C"
void battlemints_start()
{
    sranddev();

    sphere_face::global_start(); // must happen first!
    renderer::global_start(); // must happen second!
    global_start_actors();

    controller::set_current(new intro());
}

extern "C"
void battlemints_tick()
{
#ifdef BENCHMARK
    struct timeval start, end;
    gettimeofday(&start, NULL);
#endif
    controller::tick_current();
#ifdef BENCHMARK
    gettimeofday(&end, NULL);

    printf("tick: %10u\n", (int)((end.tv_sec - start.tv_sec)*1000000) + (int)((int)end.tv_usec - (int)start.tv_usec));
#endif
}

extern "C"
void battlemints_draw()
{
#ifdef BENCHMARK
    struct timeval start, end;
    gettimeofday(&start, NULL);
#endif
    controller::draw_current();
#ifdef BENCHMARK
    gettimeofday(&end, NULL);

    printf("draw: %10u\n", (int)((end.tv_sec - start.tv_sec)*1000000) + (int)((int)end.tv_usec - (int)start.tv_usec));
#endif
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

    renderer::global_finish();
    sphere_face::global_finish();
}
