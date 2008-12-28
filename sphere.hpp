#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__

#ifndef NO_GRAPHICS
#include <OpenGLES/ES1/gl.h>
#endif
#include "collision.hpp"
#include "thing.hpp"

namespace battlemints {

struct sphere : thing {
    float radius;
    float spring;

    virtual rect visibility_box();
    virtual rect collision_box();

    virtual void collide(thing &t) { t.collide_sphere(*this); }
    virtual void collide_sphere(sphere &s) { collide_sphere_sphere(*this, s); }
    virtual void collide_line(line &w) { collide_sphere_line(*this, w); }
    virtual void collide_point(thing &p) { collide_sphere_point(*this, p); }

    virtual float collision_time(thing const &t) const
        { return t.collision_time_sphere(*this); }
    virtual float collision_time_sphere(sphere const &s) const
        { return collision_time_sphere_sphere(*this, s); }
    virtual float collision_time_line(line const &w) const
        { return collision_time_sphere_line(*this, w); }
    virtual float collision_time_point(thing const &p) const
        { return collision_time_sphere_point(*this, p); }

    sphere(float m, vec2 ct, float r, float sp)
        : thing(m, ct), radius(r), spring(sp) { }

    virtual char const * kind() const { return "sphere"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " r:" << radius; }

#ifndef NO_GRAPHICS
    void accelerate_with_exhaust(vec2 accel);

protected:
    void _push_translate()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(center.x, center.y, 0.0f);
    }
#endif
};

}

#endif
