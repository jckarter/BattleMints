#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "game.hpp"
#include "sphere.hpp"

namespace battlemints {

struct player : sphere {

    player(vec2 center) : sphere(1.0, center, 0.5, make_vec4(0.6, 0.7, 0.9, 1.0), 1.0) { }

    virtual bool does_ticks() const { return true; }

    virtual void tick();

    virtual char const * kind() const { return "player"; }

    static thing *from_json(Json::Value const &v);
};

}

#endif
