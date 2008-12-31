#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "game.hpp"
#include "sphere.hpp"
#include "sphere_face.hpp"
#include "drawing.hpp"
#include "transition.hpp"
#include "explosion.hpp"
#include "board.hpp"
#include <boost/optional.hpp>

namespace battlemints {

struct player : sphere {
    static const float ACCEL_SCALE;
    static const float RADIUS;
    static const vec4 COLOR;
    static sphere_texture *texture;
    static sphere_face *face;

    player(vec2 center) : sphere(0.5, center, RADIUS, 1.0) { }

    virtual void tick();

    virtual char const * kind() const { return "player"; }

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    virtual void draw();

    void die() { explosion::explode(this); board::restart_with<death_transition>(); }

    static thing *from_json(Json::Value const &v);
    static void global_start();
    static void global_finish();

private:
    vec2 _cur_accel();

};

}

#endif
