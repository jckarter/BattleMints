#ifndef __THING_HPP__
#define __THING_HPP__

#include "geom.hpp"

#include <vector>
#include <boost/array.hpp>
#include <boost/utility.hpp>

namespace battlemints {

struct sphere;

struct thing : boost::noncopyable {
    static const boost::array<float, 8> unit_texcoords;

    vec2 velocity;
    vec2 center;
    float mass;

    thing(float m, vec2 ct) : velocity(make_vec2(0.0)), center(ct), mass(m) { }

    virtual ~thing() { }
    virtual rect visibility_box() = 0;
    virtual rect collision_box() = 0;
    virtual void draw() = 0;

    virtual bool does_ticks() const { return false; }
    virtual bool does_collisions() const { return true; }

    virtual void tick() { }

    virtual void collide(thing &o) = 0;
    virtual void collide_sphere(sphere &s) = 0;

    virtual float collision_time(thing const &o) const = 0;
    virtual float collision_time_sphere(sphere const &s) const = 0;
};

}

#endif
