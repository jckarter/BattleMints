#include "battlemints.h"
#include "board.hpp"
#include "thing.hpp"
#include "sphere.hpp"
#include "spring.hpp"
#include "player.hpp"
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <boost/foreach.hpp>
#include <cstdlib>
#include <cstdio>

namespace battlemints {

vec2 controller_state;

board *game_board;

float rand_between(float lo, float hi)
{
    float r = (float)rand()/(float)RAND_MAX;
    float spread = hi - lo;
    float ret = r * spread + lo;
    
    return ret;
}

}

using namespace battlemints;

extern "C"
void battlemints_start()
{
    sranddev();

    game_board = new board();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(
        -160.0f * GAME_UNITS_PER_PIXEL, 160.0f * GAME_UNITS_PER_PIXEL,
        -240.0f * GAME_UNITS_PER_PIXEL, 240.0f * GAME_UNITS_PER_PIXEL,
        -2.0f, 2.0f
    );

    for (int i = 0; i < 300; ++i) {
        float radius = rand_between(0.25f, 0.75f);

        sphere *s = new sphere(
            2.0 * radius,
            make_vec2(rand_between(-24.0f, 24.0f), rand_between(-24.0f, 24.0f)),
            radius,
            make_vec4(rand_between(0.8f, 1.0f), rand_between(0.8f, 1.0f), rand_between(0.8f, 1.0f), 1.0f)
        );

        game_board->add_thing(s);
    }

    for (int i = 0; i < 50; ++i) {
        spring *s = new spring(
            make_vec2(rand_between(-32.0f, 32.0f), rand_between(-32.0f, 32.0f))
        );

        game_board->add_thing(s);
    }

    player *p = new player();
    game_board->add_thing(p);
    game_board->set_camera(p);
}

extern "C"
void battlemints_tick()
{
    game_board->tick();
}

extern "C"
void battlemints_draw()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    game_board->draw();
}

extern "C"
void battlemints_input(int player, float x, float y, int buttons)
{
    controller_state = make_vec2(x, y);
}

extern "C"
void battlemints_finish()
{
    delete game_board;
}
