#ifndef __ENEMY_HPP__
#define __ENEMY_HPP__

#include "game.hpp"
#include "sphere.hpp"

namespace battlemints {

struct enemy : sphere {
    float accel;
    float responsiveness;
    thing *target;
    vec2 cur_accel;

    enemy(float m, vec2 ct, float r, vec4 co, float sp, float a, float re)
        : sphere(m, ct, r, co, sp), accel(a), responsiveness(re),
          target(NULL), cur_accel(make_vec2(0.0)) {}

    virtual bool does_ticks() const { return true; }

    virtual void tick();
    virtual void on_collision(thing &o);

    virtual char const * kind() const { return "enemy"; }

    static thing *from_json(Json::Value const &v);
};

}

#endif
