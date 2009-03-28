#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "geom.hpp"

namespace battlemints {

const char *INITIAL_MAP = "start";
const char *NEW_GAME_MAP = "hub";

struct thing;
struct controller;

extern const float BORDER_THICKNESS;
extern const float PIXELS_PER_GAME_UNIT;
extern const float GAME_UNITS_PER_PIXEL;
extern const float FRICTION;
extern const float MOVEMENT_THRESHOLD;
extern const float INFINITYF;
extern const vec2 ZERO_VEC2;
extern const vec4 ZERO_VEC4;
extern const vec2 GAME_WINDOW_SIZE;
extern const vec2 GAME_WINDOW_UNIT_SIZE;

const int GRACE_PERIOD = 30;

const int NUM_GOALS = 64;
const int NUM_COLOR_SWITCHES = 64;

extern vec2 controller_state;
extern int controller_tap_count;

inline float max_speed_for_accel(float accel) { return accel/(1.0f - FRICTION); }

}

#endif
