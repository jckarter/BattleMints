#ifndef __WALLS_HPP__
#define __WALLS_HPP__

#include "thing.hpp"
#include "collision.hpp"
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

struct wallpost : point {
    wallpost(vec2 ct) : point(ct) {  }

    virtual bool does_draws() const { return false; }
    //virtual void draw() {
    //    glDisable(GL_TEXTURE_2D);
    //    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //    
    //    glPointSize(3.0f);
    //    glColor4f(0.0, 1.0, 0.0, 1.0);
    //    glVertexPointer(2, GL_FLOAT, 0, (void*)&center);
    //    glDrawArrays(GL_POINTS, 0, 1);
    //}
    //virtual rect visibility_box() { return make_rect(center, center); }

    virtual void on_collision(thing &o) { o.post_damage(); }

    virtual char const * kind() const { return "wallpost"; }
};

struct wall_strip : thing {
    std::vector<vec2> vertices;
    rect bounding_box; // order dependent init
    bool closed;

    template<typename InputIterator>
    wall_strip(InputIterator start, InputIterator fin, bool cl)
        : thing(INFINITYF, ZERO_VEC2, 1.0f), vertices(start, fin), bounding_box(_find_bbox()), closed(cl) 
        { }

    virtual bool does_collisions() const { return false; }

    virtual void draw();

    virtual void awaken() { _generate_collision_things(); }

    virtual rect visibility_box() { return bounding_box; }

    virtual char const * kind() const { return "wall_strip"; }

    static thing *from_json(Json::Value const &v);

private:
    rect _find_bbox();
    void _generate_collision_things();
};

}

#endif
