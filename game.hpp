#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "geom.hpp"
#include "sound_server.hpp"

namespace battlemints {

struct thing;
struct controller;

static const float BORDER_THICKNESS = 0.045f;
static const float PIXELS_PER_GAME_UNIT = 30.0f;
static const float GAME_UNITS_PER_PIXEL = 1.0f / PIXELS_PER_GAME_UNIT;
static const float PLAYER_ACCEL_SCALE = 0.02f;
static const float FRICTION = 0.93f;
static const float MOVEMENT_THRESHOLD = 0.000001f;
static const vec2 GAME_WINDOW_SIZE = make_vec2(320.0, 480.0);
static const vec2 GAME_WINDOW_UNIT_SIZE = make_vec2(320.0, 480.0) * GAME_UNITS_PER_PIXEL;

extern vec2 controller_state;
extern sound_server *sounds;
extern sound_effect *impact_sound;

}

#endif
