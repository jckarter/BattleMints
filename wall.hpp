#ifndef __WALL_HPP__
#define __WALL_HPP__

#include "thing.hpp"
#include "line.hpp"
#include "collision.hpp"
#include <limits>
#include <boost/array.hpp>

namespace battlemints {

struct wall : line {
    wall(vec2 pt_a, vec2 pt_b) : line(pt_a, pt_b) { }

    virtual bool does_draws() const { return false; }
    virtual void on_collision(thing &o) { o.wall_damage(); }
    //virtual void draw() {
    //    glDisable(GL_TEXTURE_2D);
    //    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    //    boost::array<vec2, 2> vertices = { endpoint_a, endpoint_b };
    //
    //    glColor4f(0.0, 1.0, 0.0, 1.0);
    //    glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
    //    glDrawArrays(GL_LINES, 0, 2);
    //}
    //virtual rect visibility_box() { return collision_box(); }

    virtual char const * kind() const { return "wall"; }
};

}

#endif
