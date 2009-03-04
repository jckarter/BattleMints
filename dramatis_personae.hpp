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
#include <boost/array.hpp>
#include <iostream>
#include <vector>

namespace battlemints {

void global_start_actors();

struct player : sphere {
    static const float ACCEL_SCALE, RADIUS, MASS, SPRING;
    static const vec4 COLOR;

    static boost::array<renders_with_pair, 2> renders_with_pairs;

    player(vec2 center) : sphere(MASS, center, RADIUS, SPRING) { }

    virtual void tick();

    virtual char const * kind() const { return "player"; }

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float) { return COLOR; }

    void die()
    {
        board::current()->particles.explode(this);
        board::restart_with<death_transition>();
    }

    static thing *from_json(Json::Value const &v) { return sphere::from_json<player>(v); }
};

struct powerup : sphere {
    static const float SPIN, RADIUS, MASS, SPRING;
    static const vec4 COLOR;

    static boost::array<renders_with_pair, 1> renders_with_pairs;

    std::string powerup_kind;
    float spin;

    powerup(vec2 center, std::string const &k)
        : sphere(MASS, center, RADIUS, SPRING), powerup_kind(k), spin(0.0f) { }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float) { return COLOR; }

    virtual void tick();

    virtual char const * kind() const { return "powerup"; }

    static thing *from_json(Json::Value const &v);
};

struct enemy : sphere {
    float accel;
    float responsiveness;

    thing *target;

    enemy(float m, vec2 ct, float r, float sp, float a, float re)
        : sphere(m, ct, r, sp), accel(a), responsiveness(re),
          target(NULL) {}

    virtual void tick();
    virtual void on_collision(thing &o);

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    void die() { board::current()->particles.explode(this); }

    virtual void trigger(thing *scapegoat);

    virtual char const * kind() const { return "enemy"; }
};

struct mini : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, RESPONSIVENESS;
    static const boost::array<vec4, 6> colors;

    static boost::array<renders_with_pair, 2> renders_with_pairs;

    vec4 color;

    mini(vec2 ct)
        : enemy(MASS, ct, RADIUS, SPRING, ACCEL, RESPONSIVENESS),
          color(colors[rand() % colors.size()]) { }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float) { return color; }

    virtual char const * kind() const { return "mini"; }

    static thing *from_json(Json::Value const &v) { return sphere::from_json<mini>(v); }
};

struct mega : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, RESPONSIVENESS;
    static const vec4 COLOR;

    static boost::array<renders_with_pair, 2> renders_with_pairs;

    mega(vec2 ct)
        : enemy(MASS, ct, RADIUS, SPRING, ACCEL, RESPONSIVENESS) { }

    virtual char const * kind() const { return "mega"; }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float) { return COLOR; }

    virtual void wall_damage() { }
    virtual void post_damage() { }

    static thing *from_json(Json::Value const &v) { return sphere::from_json<mega>(v); }
};

struct bumper : sphere {
    static const float RADIUS, MASS, INNER_RADIUS, SPRING;
    static const vec4 INNER_COLOR, OUTER_COLOR;

    static boost::array<renders_with_pair, 2> renders_with_pairs_template;

    bumper(vec2 ct)
        : sphere(MASS, ct, RADIUS, SPRING) { }

    virtual char const * kind() const { return "bumper"; }

    virtual renders_with_range renders_with() const;
    virtual vec4 sphere_color(float radius);

    static thing *from_json(Json::Value const &v) { return sphere::from_json<bumper>(v); }
};

struct switch_spring : sphere {
    static const float RADIUS, MASS, SLOT_LENGTH, SPRING_FACTOR;
    static const vec4 COLOR, TRIGGERED_COLOR, SLOT_COLOR;
    static const boost::array<vec2, 4> slot_vertices;
    
    static boost::array<renders_with_pair, 2> renders_with_pairs;

    vec2 home, axis;
    float slot_matrix[16];
    bool triggered;
    thing *last_touch;

    switch_spring(vec2 ct, vec2 ax)
        : sphere(MASS, ct - ax * SLOT_LENGTH, RADIUS, 0.0f),
          home(ct), axis(ax), triggered(false),
          last_touch(NULL);
        { _set_matrix(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual void draw_self() const;

    virtual vec4 sphere_color(float) { return triggered ? TRIGGERED_COLOR : COLOR; }

    virtual char const * kind() const { return "switch_spring"; }
    
    virtual void tick();
    virtual void on_collision(thing &o);

    static thing *from_json(Json::Value const &v);

private:
    void _set_matrix();
};

}

#endif
