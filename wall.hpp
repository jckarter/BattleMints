#ifndef __WALL_HPP__
#define __WALL_HPP__

#include "thing.hpp"
#include "collision.hpp"
#include <limits>
#include <boost/array.hpp>

namespace battlemints {

struct wall : thing {
    vec2 endpoint_a, endpoint_b, normal;

    wall(vec2 pt_a, vec2 pt_b)
        : thing(std::numeric_limits<float>::infinity(), (pt_a+pt_b)/2),
          endpoint_a(pt_a), endpoint_b(pt_b), normal(vperp(vnormalize(endpoint_b - endpoint_a)))
        { _set_up_vertices(); }

    virtual rect visibility_box();
    virtual rect collision_box();
    virtual void draw();

    virtual void collide(thing &o) { o.collide_wall(*this); }
    virtual void collide_sphere(sphere &s) { collide_sphere_wall(s, *this); }
    virtual void collide_wall(wall &w) { collide_wall_wall(*this, w); }

    virtual float collision_time(thing const &o) const { return o.collision_time_wall(*this); }
    virtual float collision_time_sphere(sphere const &s) const
        { return collision_time_sphere_wall(s, *this); }
    virtual float collision_time_wall(wall const &w) const
        { return collision_time_wall_wall(*this, w); }

    virtual char const * kind() const { return "wall"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " a:" << endpoint_a << " b:" << endpoint_b << " n:" << normal; }

private:
    void _set_up_vertices();
    boost::array<float, 8> _vertices;
};

}

#endif
