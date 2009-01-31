#include "game.hpp"
#include "dramatis_personae.hpp"
#include "tiles.hpp"
#include <boost/array.hpp>
#include <limits>
#include <cmath>

namespace battlemints {

const float BORDER_THICKNESS = 0.045f;
const float PIXELS_PER_GAME_UNIT = 30.0f;
const float GAME_UNITS_PER_PIXEL = 1.0f / PIXELS_PER_GAME_UNIT;
const float FRICTION = 0.93f;
const float MOVEMENT_THRESHOLD = 0.000001f;
const float INFINITYF = std::numeric_limits<float>::infinity();
const vec2 ZERO_VEC2 = make_vec2(0.0);
const vec2 GAME_WINDOW_SIZE = make_vec2(320.0, 480.0);
const vec2 GAME_WINDOW_UNIT_SIZE = make_vec2(320.0, 480.0) * GAME_UNITS_PER_PIXEL;

const vec2 board::CELL_SIZE = make_vec2(6.0, 6.0);

const float cambot::LEAD_FACTOR = 15.0f;
const float cambot::FOLLOW_FACTOR = 0.1f;
const float cambot::ACCEL = 0.03f;

const float player::ACCEL_SCALE = 0.02f;
const float player::RADIUS = 0.5f;
const float player::MASS = 1.0f;
const float player::SPRING = 1.0f;
const vec4 player::COLOR = make_vec4(0.5, 0.89, 1.0, 1.0);

const float mini::ACCEL = 0.021f;
const float mini::RADIUS = 0.35f;
const float mini::MASS = 0.45f;
const float mini::SPRING = 1.0f;
const float mini::RESPONSIVENESS = 0.65f;

const float mega::ACCEL = 0.004f;
const float mega::RADIUS = 2.6f;
const float mega::MASS = 15.0f;
const float mega::SPRING = 1.0f;
const float mega::RESPONSIVENESS = 0.9f;
const vec4 mega::COLOR = make_vec4(0.33, 0.13, 0.0, 1.0);

const float bumper::RADIUS = 0.5f;
const float bumper::MASS = 10000.0f;
const float bumper::SPRING = 2.0f;
const float bumper::INNER_RADIUS = 0.4f;
const vec4 bumper::INNER_COLOR = make_vec4(0.8f, 1.0f, 0.0f, 1.0f);
const vec4 bumper::OUTER_COLOR = make_vec4(1.0f, 0.0f, 0.0f, 0.5f);

const float powerup::SPIN = 0.01f;
const float powerup::RADIUS = 0.5f;
const float powerup::MASS = 1000.0f;
const float powerup::SPRING = 1.0f;
const vec4 powerup::COLOR = make_vec4(1.0f, 0.86f, 0.0f, 1.0f);

vec2 controller_state;

const vec4 tile_octagon::tile_color    = make_vec4(0.96, 0.84, 0.93, 1.0);
const vec4 tile_hexagon_90::tile_color = make_vec4(0.78, 0.71, 0.75, 1.0);
const vec4 tile_hexagon::tile_color    = make_vec4(1.0,  0.96, 0.84, 1.0);
const vec4 tile_trapezoid::tile_color  = make_vec4(1.0,  0.84, 0.84, 1.0);
const vec4 tile_square::tile_color     = make_vec4(1.0,  0.90, 0.84, 1.0);
const vec4 tile_rhombus_60::tile_color = make_vec4(0.84, 0.84, 1.0,  1.0);
const vec4 tile_rhombus_45::tile_color = make_vec4(1.0,  0.66, 0.8,  1.0);
const vec4 tile_rhombus_30::tile_color = make_vec4(0.96, 0.89, 0.84, 1.0);
const vec4 tile_triangle::tile_color   = make_vec4(0.84, 1.0,  0.84, 1.0);
const vec4 arrow::tile_color           = make_vec4(0.28, 0.24, 0.22, 0.5);

const boost::array<vec2, 8> tile_octagon::tile_vertices = {
    make_vec2(0.0f,                     2.0f              ),
    make_vec2(0.0f,                     0.0f              ),
    make_vec2(            sqrtf(2.0f),  2.0f + sqrtf(2.0f)),
    make_vec2(            sqrtf(2.0f),       - sqrtf(2.0f)),
    make_vec2(2.0f +      sqrtf(2.0f),  2.0f + sqrtf(2.0f)),
    make_vec2(2.0f +      sqrtf(2.0f),       - sqrtf(2.0f)),
    make_vec2(2.0f + 2.0f*sqrtf(2.0f), -2.0f              ),
    make_vec2(2.0f + 2.0f*sqrtf(2.0f),  0.0f              ),
};

const boost::array<vec2, 6> tile_hexagon_90::tile_vertices = {
    make_vec2(0.0f,                     0.0f       ),
    make_vec2(            sqrtf(2.0f), -sqrtf(2.0f)),
    make_vec2(            sqrtf(2.0f),  sqrtf(2.0f)),
    make_vec2(2.0f +      sqrtf(2.0f), -sqrtf(2.0f)),
    make_vec2(2.0f +      sqrtf(2.0f),  sqrtf(2.0f)),
    make_vec2(2.0f + 2.0f*sqrtf(2.0f),  0.0f       )
};

const boost::array<vec2, 6> tile_hexagon::tile_vertices = {
    make_vec2(0.0f,  0.0f      ),
    make_vec2(1.0f, -sqrtf(3.0)),
    make_vec2(1.0f,  sqrtf(3.0)),
    make_vec2(3.0f, -sqrtf(3.0)),
    make_vec2(3.0f,  sqrtf(3.0)),
    make_vec2(4.0f,  0.0f      )
};

const boost::array<vec2, 4> tile_trapezoid::tile_vertices = {
    make_vec2(0.0f,  0.0f      ),
    make_vec2(1.0f, -sqrtf(3.0)),
    make_vec2(4.0f,  0.0f      ),
    make_vec2(3.0f, -sqrtf(3.0))
};

const boost::array<vec2, 4> tile_square::tile_vertices = {
    make_vec2(0.0f,  0.0f),
    make_vec2(0.0f, -2.0f),
    make_vec2(2.0f,  0.0f),
    make_vec2(2.0f, -2.0f)
};

const boost::array<vec2, 4> tile_rhombus_60::tile_vertices = {
    make_vec2(0.0f,  0.0f      ),
    make_vec2(1.0f, -sqrtf(3.0)),
    make_vec2(2.0f,  0.0f      ),
    make_vec2(3.0f, -sqrtf(3.0))
};

const boost::array<vec2, 4> tile_rhombus_45::tile_vertices = {
    make_vec2(0.0f              ,        0.0f ),
    make_vec2(       sqrtf(2.0f), -sqrtf(2.0f)),
    make_vec2(2.0f              ,        0.0f ),
    make_vec2(2.0f + sqrtf(2.0f), -sqrtf(2.0f))
};

const boost::array<vec2, 4> tile_rhombus_30::tile_vertices = {
    make_vec2(0.0f,  0.0f              ),
    make_vec2(0.0f, -2.0f              ),
    make_vec2(1.0f,        -sqrtf(3.0f)),
    make_vec2(1.0f, -2.0f - sqrtf(3.0f))
};

const boost::array<vec2, 3> tile_triangle::tile_vertices = {
    make_vec2(0.0f,  0.0f),
    make_vec2(1.0f, -sqrtf(3.0f)),
    make_vec2(2.0f,  0.0f) 
};

const boost::array<vec2, 5> arrow::tile_vertices = {
    make_vec2(-0.5f,  0.3f),
    make_vec2(-0.5f, -0.3f),
    make_vec2( 0.0f,  0.3f),
    make_vec2( 0.8f, -0.3f),
    make_vec2( 0.5f,  0.0f),
};

}

