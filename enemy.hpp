#ifndef __ENEMY_HPP__
#define __ENEMY_HPP__

#include "game.hpp"
#include "sphere.hpp"
#include "board.hpp"

namespace battlemints {

struct enemy : sphere {
    float accel;
    float responsiveness;
    thing *target;
    vec2 cur_accel;

    enemy(float m, vec2 ct, float r, vec4 co, float sp, float a, float re)
        : sphere(m, ct, r, co, sp), accel(a), responsiveness(re),
          target(NULL), cur_accel(ZERO_VEC2) {}

    virtual void tick();
    virtual void on_collision(thing &o);

    virtual void wall_damage() {
        if (board::current()->thing_lives(this))
            explosion::explode(this);
    }

    virtual char const * kind() const { return "enemy"; }

    static thing *from_json(Json::Value const &v);
};

}

#endif
