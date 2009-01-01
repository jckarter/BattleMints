#ifndef __THING_HPP__
#define __THING_HPP__

#include "geom.hpp"
#include "game.hpp"
#include "collision.hpp"

#include <vector>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <ostream>
#ifndef NO_GRAPHICS
#include <OpenGLES/ES1/gl.h>
#endif

namespace battlemints {

struct sphere;
struct line;

struct thing : boost::noncopyable {
    vec2 velocity;
    vec2 center;
    float spring;
    float mass;

    thing(float m, vec2 ct, float sp) : velocity(ZERO_VEC2), center(ct), spring(sp), mass(m) { }

    virtual ~thing() { }
    virtual rect visibility_box() { return make_rect(0,0,0,0); }
    virtual rect collision_box() { return make_rect(0,0,0,0); }
    virtual void draw() { }

    virtual bool does_draws() const { return true; }
    virtual bool does_collisions() const { return true; }
    virtual bool can_overlap() const { return false; }

    virtual void tick() { }

    /* handle physical results of collision */
    virtual void collide(thing &o) { }
    virtual void collide_sphere(sphere &s) { }
    virtual void collide_line(line &w) { }
    virtual void collide_point(thing &p) { }

    virtual float collision_time(thing const &o) const
        { return INFINITYF; }
    virtual float collision_time_sphere(sphere const &s) const
        { return INFINITYF; }
    virtual float collision_time_line(line const &w) const
        { return INFINITYF; }
    virtual float collision_time_point(thing const &p) const
        { return INFINITYF; }

    /* thing-specific reaction to collision */
    virtual void on_collision(thing &o) { }

    virtual void wall_damage() { }
    virtual void post_damage() { }

    virtual char const * kind() const { return "thing"; }
    virtual void print(std::ostream &os) const
        { os << kind() << " " << (void*)this
             << " v:" << velocity << " c:" << center << " m:" << mass; }

    virtual void awaken() { } // Called when board activates

#ifndef NO_GRAPHICS
protected:
    void _push_translate()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(center.x, center.y, 0.0f);
    }
#endif
};

static inline std::ostream &operator<<(std::ostream &os, thing const &th)
    { th.print(os); return os; }

struct sphere : thing {
    float radius;

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
        : thing(m, ct, sp), radius(r) { }

    virtual char const * kind() const { return "sphere"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " r:" << radius; }

#ifndef NO_GRAPHICS
    void accelerate_with_exhaust(vec2 accel);
#endif
};

struct line : thing {
    vec2 endpoint_a, endpoint_b, normal;

    line(vec2 pt_a, vec2 pt_b)
        : thing(INFINITYF, (pt_a+pt_b)/2, 1.0f),
          endpoint_a(pt_a), endpoint_b(pt_b), normal(vperp(vnormalize(endpoint_b - endpoint_a)))
        { }

    virtual rect visibility_box();
    virtual rect collision_box();

    virtual void collide(thing &o) { o.collide_line(*this); }
    virtual void collide_sphere(sphere &s) { collide_sphere_line(s, *this); }
    virtual void collide_line(line &w) { collide_line_line(*this, w); }
    virtual void collide_point(thing &p) { collide_line_point(*this, p); }

    virtual float collision_time(thing const &o) const { return o.collision_time_line(*this); }
    virtual float collision_time_sphere(sphere const &s) const
        { return collision_time_sphere_line(s, *this); }
    virtual float collision_time_line(line const &w) const
        { return collision_time_line_line(*this, w); }
    virtual float collision_time_point(thing const &p) const
        { return collision_time_line_point(*this, p); }

    virtual char const * kind() const { return "line"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " a:" << endpoint_a << " b:" << endpoint_b << " n:" << normal; }
};

}

#endif
