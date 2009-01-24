#ifndef __WALLS_HPP__
#define __WALLS_HPP__

#include "thing.hpp"
#include "collision.hpp"
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
};

struct wall_strip : thing {
    std::vector<vec2> vertices;
    bool closed;

    template<typename InputIterator>
    wall_strip(InputIterator start, InputIterator fin, bool cl)
        : thing(INFINITYF, ZERO_VEC2, 1.0f), vertices(start, fin), closed(cl) 
        { }

    virtual bool does_collisions() const { return false; }

    virtual void awaken() { _generate_collision_things(); }

    virtual char const * kind() const { return "wall_strip"; }

    static thing *from_json(Json::Value const &v);

private:
    void _generate_collision_things();
};

}

#endif
