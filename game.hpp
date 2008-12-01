#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "geom.hpp"
#include "sound_server.hpp"

namespace battlemints {

struct thing;
struct controller;

extern const float BORDER_THICKNESS;
extern const float PIXELS_PER_GAME_UNIT;
extern const float GAME_UNITS_PER_PIXEL;
extern const float PLAYER_ACCEL_SCALE;
extern const float FRICTION;
extern const float MOVEMENT_THRESHOLD;
extern const vec2 ZERO_VEC2;
extern const vec2 GAME_WINDOW_SIZE;
extern const vec2 GAME_WINDOW_UNIT_SIZE;

extern vec2 controller_state;

}

#endif
