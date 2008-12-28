#ifndef __ENEMY_HPP__
#define __ENEMY_HPP__

#include "game.hpp"
#include "geom.hpp"
#include "sphere.hpp"
#include "board.hpp"
#include "explosion.hpp"
#include "serialization.hpp"

namespace battlemints {

struct enemy : sphere {
    float accel;
    float responsiveness;

    thing *target;
    vec2 cur_accel;

    enemy(float m, vec2 ct, float r, float sp, float a, float re)
        : sphere(m, ct, r, sp), accel(a), responsiveness(re),
          target(NULL), cur_accel(ZERO_VEC2) {}

    virtual void tick();
    virtual void on_collision(thing &o);

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    void die() { explosion::explode(this); }

    virtual char const * kind() const { return "enemy"; }

protected:
    template<typename Enemy>
    static thing *from_json(Json::Value const &v)
    {
        vec2 center = vec2_from_json(v["center"]);
        return new Enemy(center);
    }
};

}

#endif
