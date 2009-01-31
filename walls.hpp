#ifndef __WALLS_HPP__
#define __WALLS_HPP__

#include "thing.hpp"
#include "collision.hpp"
#include "serialization.hpp"
#include <boost/array.hpp>

namespace battlemints {

struct wall : line {
    wall(vec2 pt_a, vec2 pt_b) : line(pt_a, pt_b) { }

    virtual void on_collision(thing &o) { o.wall_damage(); }
    virtual void draw() {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        boost::array<vec2, 2> vertices = { endpoint_a, endpoint_b };
    
        glColor4f(0.0, 1.0, 0.0, 1.0);
        glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
        glDrawArrays(GL_LINES, 0, 2);
    }

    virtual char const * kind() const { return "wall"; }

    static thing *from_json(Json::Value const &v) { return line::from_json<wall>(v); }
};

struct wallpost : point {
    wallpost(vec2 ct) : point(ct) {  }

    virtual void draw() {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glPointSize(3.0f);
        glColor4f(0.0, 1.0, 0.0, 1.0);
        glVertexPointer(2, GL_FLOAT, 0, (void*)&center);
        glDrawArrays(GL_POINTS, 0, 1);
    }

    virtual void on_collision(thing &o) { o.post_damage(); }

    virtual char const * kind() const { return "wallpost"; }

    static thing *from_json(Json::Value const &v) { return point::from_json<wallpost>(v); }
};

}

#endif
