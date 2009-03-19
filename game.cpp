#include "game.hpp"
#include "dramatis_personae.hpp"
#include "renderers.hpp"
#include "tiles.hpp"
#include "walls.hpp"
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
const vec2 ZERO_VEC2 = CONST_VEC2_SPLAT(0.0);
const vec4 ZERO_VEC4 = CONST_VEC4_SPLAT(0.0);
const vec2 GAME_WINDOW_SIZE = CONST_VEC2(320.0, 480.0);
const vec2 GAME_WINDOW_UNIT_SIZE = CONST_VEC2(320.0, 480.0) * GAME_UNITS_PER_PIXEL;

const float sphere_face::ROTATE_SPAN = 20.0f,
            sphere_face::ROTATE_FACTOR = 40.0f;

const vec2 board::CELL_SIZE = CONST_VEC2(6.0, 6.0);
const float board::LIVE_RADIUS = 30.0;

const float cambot::LEAD_FACTOR = 15.0f;
const float cambot::FOLLOW_FACTOR = 0.1f;
const float cambot::ACCEL = 0.03f;

const float sphere::EXHAUST_FACTOR = 2.0f;

const float player::ACCEL_SCALE = 0.014f;
const float player::RADIUS = 0.5f;
const float player::MASS = 1.0f;
const float player::SPRING = 0.0f;
const float player::DAMP = 1.0f;
const vec4  player::COLOR = CONST_VEC4(0.5f, 0.89f, 1.0f, 1.0f);
const float player::SHIELD_RADIUS = 0.6f;
const float player::SHIELD_SPRING = 0.3f;
const vec4  player::SHIELD_COLOR = CONST_VEC4(1.0f, 0.25f, 0.5f, 0.75f);
const float player::INVULN_SPRING = 1.0f;
const float player::INVULN_MASS = 100.0f;
const float player::INVULN_DAMP = 0.0f;
const vec4  player::INVULN_BODY_COLOR = CONST_VEC4(0.8f, 0.95f, 1.0f, 1.0f);
const float player::PANIC_SPRING = 0.75f;
const float player::PANIC_RADIUS = 1.5f;
const float player::PANIC_MASS = 50.0f;
const vec4  player::PANIC_COLOR = CONST_VEC4(1.0f, 0.89f, 1.0f, 1.0f);

const boost::array<vec4, 6> player::invuln_colors = {
    make_vec4(1.0f, 0.0f, 0.0f, 0.75f),
    make_vec4(0.0f, 1.0f, 0.0f, 0.75f),
    make_vec4(1.0f, 1.0f, 0.0f, 0.75f),
    make_vec4(0.0f, 0.0f, 1.0f, 0.75f),
    make_vec4(1.0f, 0.0f, 1.0f, 0.75f),
    make_vec4(0.0f, 1.0f, 1.0f, 0.75f)
};

const float mini::ACCEL = 0.013f;
const float mini::RADIUS = 0.35f;
const float mini::MASS = 0.4f;
const float mini::SPRING = 0.005f;
const float mini::DAMP = 1.5f;
const float mini::RESPONSIVENESS = 0.6f;

const boost::array<vec4, 6> mini::colors = {
    make_vec4(1.0, 0.0,  0.0, 1.0),
    make_vec4(1.0, 0.4,  0.0, 1.0),
    make_vec4(1.0, 1.0,  0.0, 1.0),
    make_vec4(0.0, 1.0,  0.0, 1.0),
    make_vec4(0.3, 0.3,  1.0, 1.0),
    make_vec4(0.3, 0.24, 0.2, 1.0)
};

const float mega::ACCEL = 0.002f;
const float mega::RADIUS = 2.6f;
const float mega::MASS = 20.0f;
const float mega::SPRING = 0.0f;
const float mega::DAMP = 0.28f;
const float mega::RESPONSIVENESS = 0.9f;
const float mega::DEATH_THRESHOLD2 = 0.15f*0.15f;
const vec4  mega::COLOR = CONST_VEC4(0.33, 0.13, 0.0, 1.0);

const float bumper::RADIUS = 0.5f;
const float bumper::MASS = 10000.0f;
const float bumper::SPRING = 0.30f;
const float bumper::DAMP = 0.0f;
const float bumper::INNER_RADIUS = 0.4f;
const vec4  bumper::INNER_COLOR = CONST_VEC4(0.8f, 1.0f, 0.0f, 1.0f);
const vec4  bumper::OUTER_COLOR = CONST_VEC4(1.0f, 0.0f, 0.0f, 0.5f);

const float powerup::RADIUS = 0.5f;
const float powerup::MASS = 10000.0f;
const float powerup::SPRING = 0.0f;
const float powerup::DAMP = 0.0f;
const vec4  powerup::CHARGED_COLOR = CONST_VEC4(1.0f, 0.86f, 0.0f, 1.0f);
const vec4  powerup::DEAD_COLOR = CONST_VEC4(0.65f, 0.64f, 0.60f, 1.0f);

const float pellet::RADIUS = 0.1f;
const boost::array<vec4, 6> pellet::colors = {
    make_vec4(1.0f, 0.0f, 0.0f, 0.50f),
    make_vec4(0.0f, 1.0f, 0.0f, 0.50f),
    make_vec4(1.0f, 1.0f, 0.0f, 0.50f),
    make_vec4(0.0f, 0.0f, 1.0f, 0.50f),
    make_vec4(1.0f, 0.0f, 1.0f, 0.50f),
    make_vec4(0.0f, 1.0f, 1.0f, 0.50f)
};

const boost::array<vec4, 2> powerup::pulse_colors = {
    CONST_VEC4(1.0f, 0.0f, 0.0f, 1.0f),
    CONST_VEC4(1.0f, 1.0f, 1.0f, 1.0f),
};

const float switch_spring::RADIUS = 0.2f;
const float switch_spring::MASS = 0.1f;
const float switch_spring::SLOT_LENGTH = 0.5f;
const float switch_spring::SLOT_WIDTH = 0.05f;
const float switch_spring::SPRING_FACTOR = 0.4f;

const vec4 switch_spring::COLOR = CONST_VEC4(0.89f, 0.87f, 0.86f, 1.0f);
const vec4 switch_spring::TRIGGERED_COLOR = CONST_VEC4(0.69f, 1.00f, 0.66f, 1.0f);
const vec4 switch_spring::SLOT_COLOR = CONST_VEC4(0.43f, 0.36f, 0.33f, 1.0f);

const vec4 door::CLUMP_COLOR = CONST_VEC4(1.0f, 0.88f, 0.88f, 1.0f);
const vec4 door::COLOR = CONST_VEC4(1.0f, 0.66f, 0.66f, 1.0f);

const boost::array<vec2, 4> switch_spring::slot_vertices = {
    make_vec2(-SLOT_LENGTH, -SLOT_WIDTH),
    make_vec2( SLOT_LENGTH, -SLOT_WIDTH),
    make_vec2(-SLOT_LENGTH,  SLOT_WIDTH),
    make_vec2( SLOT_LENGTH,  SLOT_WIDTH)
};

vec2 controller_state;
int controller_tap_count;

}
