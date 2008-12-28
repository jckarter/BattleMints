#ifndef __WALLPOST_HPP__
#define __WALLPOST_HPP__

#include "thing.hpp"
#include "collision.hpp"

namespace battlemints {

struct wallpost : thing {
    wallpost(vec2 ct) : thing(INFINITYF, ct) {  }

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

    virtual rect collision_box() { return make_rect(center, center); }

    virtual void collide(thing &t) { t.collide_point(*this); }
    virtual void collide_sphere(sphere &s) { collide_sphere_point(s, *this); }
    virtual void collide_line(line &w) { collide_line_point(w, *this); }
    virtual void collide_point(thing &p) { collide_point_point(*this, p); }

    virtual float collision_time(thing const &t) const
        { return t.collision_time_point(*this); }
    virtual float collision_time_sphere(sphere const &s) const
        { return collision_time_sphere_point(s, *this); }
    virtual float collision_time_line(line const &w) const
        { return collision_time_line_point(w, *this); }
    virtual float collision_time_point(thing const &p) const
        { return collision_time_point_point(*this, p); }

    virtual void on_collision(thing &o) { o.post_damage(); }

    virtual char const * kind() const { return "wallpost"; }
};

}

#endif
