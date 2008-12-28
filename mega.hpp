#ifndef __MEGA_HPP__
#define __MEGA_HPP__

#include "enemy.hpp"
#include "drawing.hpp"
#include <cstdlib>

namespace battlemints {

struct mega : enemy {
    static const float ACCEL;
    static const float RADIUS;

    static sphere_texture *texture;

    mega(vec2 ct)
        : enemy(10.0, ct, RADIUS, 1.0, ACCEL, 0.75) { }

    virtual char const * kind() const { return "mega"; }

    virtual void draw() { _push_translate(); texture->draw(); glPopMatrix(); }
    virtual void wall_damage() { }

    static thing *from_json(Json::Value const &v) { return enemy::from_json<mega>(v); }
    static void global_start();
    static void global_finish();
};

}

#endif
