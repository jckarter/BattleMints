#ifndef __WALL_HPP__
#define __WALL_HPP__

#include "thing.hpp"
#include "line.hpp"
#include "collision.hpp"
#include <limits>
#include <boost/array.hpp>

namespace battlemints {

struct wall : line {
    wall(vec2 pt_a, vec2 pt_b) : line(pt_a, pt_b) { _set_up_vertices(); }

    virtual void draw();
    virtual void on_collision(thing &o);

    virtual char const * kind() const { return "wall"; }

    static thing *from_json(Json::Value const &v);

private:
    void _set_up_vertices();
    boost::array<float, 4> _vertices;
};

}

#endif
