#include "battlemints.h"
#include "controller.hpp"
#include "decorations.hpp"
#include "drawing.hpp"
#include "game.hpp"
#include "geom.hpp"
#include "intro.hpp"
#include "synth.hpp"
#include "tripwire.hpp"
#include "dramatis_personae.hpp"
#include "renderers.hpp"
#include "universe.hpp"
#include "walls.hpp"
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

    universe::set_default_name();
    if (universe::name)
        universe::instance.load(*universe::name);
    else {
        universe::instance.set_default();
        universe::instance.set_current_map(board_name::make(0, 0));
    }

    sphere_face::global_start(); // must happen first!
    renderer::global_start();    // must happen second!
    global_start_tripwires();
    global_start_actors();
    global_start_decorations();
    global_start_walls();

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
void battlemints_input(float x, float y, int tap_count)
{
    controller_state = make_vec2(x, y);
    controller_tap_count = tap_count;
}

extern "C"
void battlemints_finish()
{
    controller::delete_current();

    global_finish_tripwires();
    renderer::global_finish();
    sphere_face::global_finish();
}

extern "C"
void battlemints_go_to_hub()
{
    if (board::current()) {
        board::change_board_with<fadeout_transition>(board::current()->name.with_stage(0));
    }
}

extern "C"
void battlemints_go_to_start()
{
    if (board::current()) {
        universe::clear_name();
        board::change_board_with<fadeout_transition>(board_name::make(0, 0));
    }
}

extern "C"
int battlemints_pause_flags()
{
    if (board::current()) {
        return BATTLEMINTS_GAME_ACTIVE
            |  (board::current()->name == "hub"   ? BATTLEMINTS_HUB_MAP   : 0)
            |  (board::current()->name == "start" ? BATTLEMINTS_START_MAP : 0);
    } else
        return 0;
}

