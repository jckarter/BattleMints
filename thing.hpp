#ifndef __THING_HPP__
#define __THING_HPP__

#include "geom.hpp"
#include "game.hpp"

#include <vector>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <ostream>

namespace battlemints {

struct sphere;
struct line;

struct thing : boost::noncopyable {
    vec2 velocity;
    vec2 center;
    float mass;

    thing(float m, vec2 ct) : velocity(ZERO_VEC2), center(ct), mass(m) { }

    virtual ~thing() { }
    virtual rect visibility_box() { return make_rect(0,0,0,0); }
    virtual rect collision_box() { return make_rect(0,0,0,0); }
    virtual void draw() { }

    virtual bool does_draws() const { return true; }
    virtual bool does_collisions() const { return true; }
    virtual bool can_overlap() const { return false; }

    virtual void tick() { }

    /* handle physical results of collision */
    virtual void collide(thing &o) { }
    virtual void collide_sphere(sphere &s) { }
    virtual void collide_line(line &w) { }
    virtual void collide_point(thing &p) { }

    virtual float collision_time(thing const &o) const
        { return INFINITYF; }
    virtual float collision_time_sphere(sphere const &s) const
        { return INFINITYF; }
    virtual float collision_time_line(line const &w) const
        { return INFINITYF; }
    virtual float collision_time_point(thing const &p) const
        { return INFINITYF; }

    /* thing-specific reaction to collision */
    virtual void on_collision(thing &o) { }

    virtual void wall_damage() { }
    virtual void post_damage() { }

    virtual char const * kind() const { return "thing"; }
    virtual void print(std::ostream &os) const
        { os << kind() << " " << (void*)this
             << " v:" << velocity << " c:" << center << " m:" << mass; }

    virtual void awaken() { } // Called when board activates
};

static inline std::ostream &operator<<(std::ostream &os, thing const &th)
    { th.print(os); return os; }

}

#endif
