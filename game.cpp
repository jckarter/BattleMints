#include "game.hpp"
#include "dramatis_personae.hpp"
#include "decorations.hpp"
#include "renderers.hpp"
#include "tiles.hpp"
#include "walls.hpp"
#include <boost/array.hpp>
#include <limits>
#include <cmath>

namespace battlemints {

const char *INITIAL_MAP = "start";
const char *NEW_GAME_MAP = "hub";

const float BORDER_THICKNESS = 0.045f;
const float PIXELS_PER_GAME_UNIT = 30.0f;
const float GAME_UNITS_PER_PIXEL = 1.0f / PIXELS_PER_GAME_UNIT;
const float FRICTION = 0.93f;
const float MOVEMENT_THRESHOLD = 0.000001f;
const float INFINITYF = std::numeric_limits<float>::infinity();
const vec2 ZERO_VEC2 = make_vec2(0.0);
const vec4 ZERO_VEC4 = make_vec4(0.0);
const vec2 GAME_WINDOW_SIZE = make_vec2(320.0, 480.0);
const vec2 GAME_WINDOW_UNIT_SIZE = make_vec2(320.0, 480.0) * GAME_UNITS_PER_PIXEL;

const float sphere_face::ROTATE_SPAN = 20.0f,
            sphere_face::ROTATE_FACTOR = 0.56f;

const vec2 board::CELL_SIZE = make_vec2(5.5, 5.5);
const float board::LIVE_RADIUS = 15.0;

const float cambot::LEAD_FACTOR = 15.0f;
const float cambot::FOLLOW_FACTOR = 0.1f;
const float cambot::ACCEL = 0.03f;

const float sphere::EXHAUST_FACTOR = 2.0f;

const float player::ACCEL_SCALE = 0.014f;
const float player::RADIUS = 0.5f;
const float player::MASS = 1.0f;
const float player::SPRING = 0.0f;
const float player::DAMP = 1.0f;
const vec4  player::COLOR = make_vec4(0.5f, 0.89f, 1.0f, 1.0f);
const float player::SHIELD_RADIUS = 0.6f;
const float player::SHIELD_SPRING = 0.3f;
const vec4  player::SHIELD_COLOR = make_vec4(1.0f, 0.25f, 0.5f, 0.75f);
const float player::INVULN_SPRING = 1.0f;
const float player::INVULN_MASS = 100.0f;
const float player::INVULN_DAMP = 0.0f;
const vec4  player::INVULN_BODY_COLOR = make_vec4(0.8f, 0.95f, 1.0f, 1.0f);
const float player::PANIC_SPRING = 0.75f;
const float player::PANIC_RADIUS = 1.5f;
const float player::PANIC_MASS = 50.0f;
const vec4  player::PANIC_COLOR = make_vec4(1.0f, 0.89f, 1.0f, 1.0f);

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

const float shield_mini::OUTER_RADIUS = 0.45f;
const float shield_mini::SHIELD_SPRING = 0.3f;
const vec4 shield_mini::OUTER_COLOR = make_vec4(1.0f, 0.25f, 0.5f, 0.75f);

const boost::array<vec4, 6> mini::colors = {
    make_vec4(1.0, 0.0,  0.0, 1.0),
    make_vec4(1.0, 0.4,  0.0, 1.0),
    make_vec4(1.0, 1.0,  0.0, 1.0),
    make_vec4(0.0, 1.0,  0.0, 1.0),
    make_vec4(0.3, 0.3,  1.0, 1.0),
    make_vec4(0.3, 0.24, 0.2, 1.0)
};

const float durian::ACCEL = 0.003f;
const float durian::RADIUS = 0.35f;
const float durian::MASS = 0.4f;
const float durian::SPRING = 0.15f;
const float durian::DAMP = 1.5f;
const float durian::RESPONSIVENESS = 0.9f;
const vec4 durian::COLOR = make_vec4(0.78f, 0.67f, 0.22f, 1.0f);

const float mega::ACCEL = 0.002f;
const float mega::RADIUS = 2.6f;
const float mega::MASS = 20.0f;
const float mega::SPRING = 0.0f;
const float mega::DAMP = 0.28f;
const float mega::RESPONSIVENESS = 0.9f;
const float mega::DEATH_THRESHOLD2 = 0.15f*0.15f;
const vec4  mega::COLOR = make_vec4(0.33, 0.13, 0.0, 1.0);

const float bomb::ACCEL = 0.005f;
const float bomb::RADIUS = 0.35f;
const float bomb::MASS = 0.7f;
const float bomb::SPRING = 0.0f;
const float bomb::DAMP = 1.5f;
const float bomb::RESPONSIVENESS = 0.9f;
const float bomb::EXPLOSION_RADIUS = 3.0f;
const float bomb::EXPLOSION_SPRING = 1.00f;
const float bomb::EXPLOSION_MASS = 500.0f;
const float bomb::BLUSH_FACTOR = 1.0f/3000.0f;

const vec4 bomb::COLOR = make_vec4(0.14f, 0.12f, 0.11f, 1.0f);
const vec4 bomb::BLUSH_COLOR = make_vec4(0.78f, 0.22f, 0.22f, 1.0f);

const float bumper::RADIUS = 0.5f;
const float bumper::MASS = 10000.0f;
const float bumper::SPRING = 0.30f;
const float bumper::DAMP = 0.0f;
const float bumper::INNER_RADIUS = 0.4f;
const vec4  bumper::INNER_COLOR = make_vec4(0.8f, 1.0f, 0.0f, 1.0f);
const vec4  bumper::OUTER_COLOR = make_vec4(1.0f, 0.0f, 0.0f, 0.5f);

const float powerup::RADIUS = 0.5f;
const float powerup::MASS = 10000.0f;
const float powerup::SPRING = 0.0f;
const float powerup::DAMP = 0.0f;
const vec4  powerup::CHARGED_COLOR = make_vec4(1.0f, 0.86f, 0.0f, 1.0f);
const vec4  powerup::DEAD_COLOR = make_vec4(0.65f, 0.64f, 0.60f, 1.0f);

const float pellet::RADIUS = 0.1f;

const float loose_pellet::MASS = 0.005f;
const float loose_pellet::SPRING = 0.0f;
const float loose_pellet::DAMP = 0.0f;
const float loose_pellet::MIN_SPEED = 0.6f;
const float loose_pellet::MAX_SPEED = 1.0f;

const boost::array<vec4, 2> pellet::colors = {
    make_vec4(0.00f, 0.13f, 0.33f, 0.6f),
    make_vec4(1.00f, 1.00f, 1.00f, 0.6f),
};

const boost::array<vec4, 2> powerup::pulse_colors = {
    make_vec4(1.0f, 0.0f, 0.0f, 1.0f),
    make_vec4(1.0f, 1.0f, 1.0f, 1.0f),
};

const float switch_base::SLOT_LENGTH = 0.5f;
const float switch_base::SLOT_WIDTH = 0.05f;
const float switch_base::SPRING_FACTOR = 0.2f;
const vec4 switch_base::SLOT_COLOR = make_vec4(0.43f, 0.36f, 0.33f, 1.0f);

const vec4 trigger_switch::COLOR = make_vec4(0.89f, 0.87f, 0.86f, 1.0f);
const vec4 trigger_switch::TRIGGERED_COLOR = make_vec4(0.69f, 1.00f, 0.66f, 1.0f);
const float trigger_switch::RADIUS = 0.2f;
const float trigger_switch::MASS = 0.1f;

const vec4 heavy_switch::COLOR = make_vec4(0.14f, 0.12f, 0.11f, 1.0f);
const vec4 heavy_switch::TRIGGERED_COLOR = make_vec4(0.69f, 1.00f, 0.66f, 1.0f);
const float heavy_switch::RADIUS = 0.4f;
const float heavy_switch::MASS = 0.6f;

const vec4 eraser_switch::COLOR = make_vec4(1.0f, 0.0f, 0.0f, 1.0f);

const vec4 door::CLUMP_COLOR = make_vec4(1.0f, 0.88f, 0.88f, 1.0f);
const vec4 door::COLOR = make_vec4(1.0f, 0.66f, 0.66f, 1.0f);

const boost::array<vec2, 4> switch_base::slot_vertices = {
    make_vec2(-SLOT_LENGTH, -SLOT_WIDTH),
    make_vec2( SLOT_LENGTH, -SLOT_WIDTH),
    make_vec2(-SLOT_LENGTH,  SLOT_WIDTH),
    make_vec2( SLOT_LENGTH,  SLOT_WIDTH)
};

const float
    flag::FLAP_MIN_AMPLITUDE = 0.001f,
    flag::FLAP_AMPLITUDE_DAMP = 0.95f,
    flag::FLAP_MIN_INITIAL_AMPLITUDE = 0.02f,
    flag::FLAP_MAX_INITIAL_AMPLITUDE = 0.08f,
    flag::FLAP_MIN_WAVELENGTH = 1.0f,
    flag::FLAP_MAX_WAVELENGTH = 2.0f,
    flag::BASE_FLAP_MIN_INITIAL_AMPLITUDE = 0.02f,
    flag::BASE_FLAP_MAX_INITIAL_AMPLITUDE = 0.06f,
    flag::BASE_FLAP_MIN_WAVELENGTH = 1.0f,
    flag::BASE_FLAP_MAX_WAVELENGTH = 1.6f,
    flag::FLAP_PHASE_STEP = 0.02f,
    flag::WIDTH = 1.6f,
    flag::HEIGHT = 1.0f,
    flag::HEIGHT_OFFSET = 0.7f;

vec2 controller_state;
int controller_tap_count;

}
