#ifndef __THING_HPP__
#define __THING_HPP__

#include "geom.hpp"

#include <vector>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <ostream>

namespace battlemints {

struct sphere;
struct wall;

struct thing : boost::noncopyable {
    static const boost::array<float, 8> unit_texcoords;

    vec2 velocity;
    vec2 center;
    float mass;

    thing(float m, vec2 ct) : velocity(make_vec2(0.0)), center(ct), mass(m) { }

    virtual ~thing() { }
    virtual rect visibility_box() { return make_rect(0,0,0,0); }
    virtual rect collision_box() { return make_rect(0,0,0,0); }
    virtual void draw() { }

    virtual bool does_draws() const { return true; }
    virtual bool does_ticks() const { return false; }
    virtual bool does_collisions() const { return true; }

    virtual void tick() { }

    /* handle physical results of collision */
    virtual void collide(thing &o) { }
    virtual void collide_sphere(sphere &s) { }
    virtual void collide_wall(wall &w) { }

    virtual float collision_time(thing const &o) const
        { return std::numeric_limits<float>::infinity(); }
    virtual float collision_time_sphere(sphere const &s) const
        { return std::numeric_limits<float>::infinity(); }
    virtual float collision_time_wall(wall const &s) const
        { return std::numeric_limits<float>::infinity(); }

    /* thing-specific reaction to collision */
    virtual void on_collision(thing &o) { }

    virtual char const * kind() const { return "thing"; }
    virtual void print(std::ostream &os) const
        { os << kind() << " " << (void*)this
             << " v:" << velocity << " c:" << center << " m:" << mass; }
};

static inline std::ostream &operator<<(std::ostream &os, thing const &th)
    { th.print(os); return os; }

}

#endif
