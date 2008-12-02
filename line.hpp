#ifndef __LINE_HPP__
#define __LINE_HPP__

#include "thing.hpp"
#include "collision.hpp"
#include <limits>
#include <boost/array.hpp>

namespace battlemints {

struct line : thing {
    vec2 endpoint_a, endpoint_b, normal;

    line(vec2 pt_a, vec2 pt_b)
        : thing(std::numeric_limits<float>::infinity(), (pt_a+pt_b)/2),
          endpoint_a(pt_a), endpoint_b(pt_b), normal(vperp(vnormalize(endpoint_b - endpoint_a)))
        { }

    virtual rect visibility_box();
    virtual rect collision_box();

    virtual void collide(thing &o) { o.collide_line(*this); }
    virtual void collide_sphere(sphere &s) { collide_sphere_line(s, *this); }
    virtual void collide_line(line &w) { collide_line_line(*this, w); }

    virtual float collision_time(thing const &o) const { return o.collision_time_line(*this); }
    virtual float collision_time_sphere(sphere const &s) const
        { return collision_time_sphere_line(s, *this); }
    virtual float collision_time_line(line const &w) const
        { return collision_time_line_line(*this, w); }

    virtual char const * kind() const { return "line"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " a:" << endpoint_a << " b:" << endpoint_b << " n:" << normal; }
};

}

#endif
