#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "game.hpp"
#include "sphere.hpp"

namespace battlemints {

struct player : sphere {

    player() : sphere(3.0, make_vec2(0.0, 0.0), 0.5, make_vec4(1.0, 0.0, 0.0, 1.0))
        { }

    virtual bool does_ticks() const { return true; }

    virtual void tick() { velocity += controller_state * make_vec2(PLAYER_ACCEL_SCALE); }

    virtual char const * kind() const { return "player"; }
};

}

#endif
