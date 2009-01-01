#include "game.hpp"
#include "dramatis_personae.hpp"

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

const float player::ACCEL_SCALE = 0.02f;
const float player::RADIUS = 0.5f;
const float player::MASS = 1.0f;
const float player::SPRING = 1.0f;
const vec4 player::COLOR = make_vec4(0.5, 0.89, 1.0, 1.0);

const float mini::ACCEL = 0.022f;
const float mini::RADIUS = 0.35f;
const float mini::MASS = 0.5f;
const float mini::SPRING = 1.0f;
const float mini::RESPONSIVENESS = 0.75f;

const float mega::ACCEL = 0.006f;
const float mega::RADIUS = 2.6f;
const float mega::MASS = 15.0f;
const float mega::SPRING = 1.0f;
const float mega::RESPONSIVENESS = 0.9f;
const vec4 mega::COLOR = make_vec4(0.33, 0.13, 0.0, 1.0);

vec2 controller_state;

}

