#ifndef __DRAMATIS_PERSONAE_HPP__
#define __DRAMATIS_PERSONAE_HPP__

#include "game.hpp"
#include "geom.hpp"
#include "thing.hpp"
#include "board.hpp"
#include "particles.hpp"
#include "serialization.hpp"
#include "drawing.hpp"
#include "transition.hpp"
#include <iostream>
#include <vector>

namespace battlemints {

struct camera : thing {
    static const float LEAD_FACTOR, FOLLOW_FACTOR, ACCEL;

    thing *target;

    camera() : thing(0.0f, ZERO_VEC2, 0.0f), target(NULL) { }

    virtual bool does_draws() const { return false; }
    virtual bool does_collisions() const { return false; }

    virtual void tick();

    void cut_to_target(thing *new_target) { target = new_target; center = new_target->center; }
};

struct player : sphere {
    static const float ACCEL_SCALE, RADIUS, MASS, SPRING;
    static const vec4 COLOR;
    static sphere_texture *texture;
    static sphere_face *face;

    player(vec2 center) : sphere(MASS, center, RADIUS, SPRING) { }

    virtual void tick();

    virtual char const * kind() const { return "player"; }

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    virtual void draw();

    void die() { board::current()->particles_thing()->explode(this); board::restart_with<death_transition>(); }

    static thing *from_json(Json::Value const &v);
    static void global_start();
    static void global_finish();

private:
    vec2 _cur_accel();
};

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

    void die() { board::current()->particles_thing()->explode(this); }

    virtual char const * kind() const { return "enemy"; }
};

struct mini : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, RESPONSIVENESS;

    static sphere_texture *texture;
    static sphere_face *face;
    static boost::array<vec4, 6> colors;

    vec4 color;

    mini(vec2 ct)
        : enemy(MASS, ct, RADIUS, SPRING, ACCEL, RESPONSIVENESS),
          color(colors[rand() % colors.size()]) { }

    virtual char const * kind() const { return "mini"; }

    virtual void draw();
    static thing *from_json(Json::Value const &v) { return thing::from_json<mini>(v); }
    static void global_start();
    static void global_finish();
};

struct mega : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, RESPONSIVENESS;
    static const vec4 COLOR;

    static sphere_texture *texture;
    static sphere_face *face;

    mega(vec2 ct)
        : enemy(MASS, ct, RADIUS, SPRING, ACCEL, RESPONSIVENESS) { }

    virtual char const * kind() const { return "mega"; }

    virtual void draw();
    virtual void wall_damage() { }
    virtual void post_damage() { }

    static thing *from_json(Json::Value const &v) { return thing::from_json<mega>(v); }
    static void global_start();
    static void global_finish();
};

struct bumper : sphere {
    static const float RADIUS, INNER_RADIUS, SPRING, INNER_COLOR, OUTER_COLOR;

    static sphere_texture *inner_texture, *outer_texture;

    bumper(vec2 ct)
        : sphere(INFINITYF, ct, RADIUS, SPRING) { }

    virtual char const * kind() const { return "bumper"; }

    virtual void draw();

    static thing *from_json(Json::Value const &v) { return thing::from_json<bumper>(v); }
    static void global_start();
    static void global_finish();
};

struct spring : sphere {
    vec2 home;
    float factor;

    spring(float m, vec2 ct, float r, float sp, float f)
        : sphere(m, ct, r, sp), home(ct), factor(f) { }

    virtual char const * kind() const { return "bumper"; }

    virtual void tick();
};

struct direction_spring : sphere {
    vec2 home, axis;
    float axis_factor, perpendicular_factor;

    direction_spring(float m, vec2 ct, float r, float sp, vec2 ax, float af, float pf)
        : sphere(m, ct, r, sp), home(ct), axis(ax), axis_factor(af), perpendicular_factor(pf) { }

    virtual char const * kind() const { return "direction_spring"; }

    virtual void tick();
};

}


#endif
