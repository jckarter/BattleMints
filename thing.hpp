#ifndef __THING_HPP__
#define __THING_HPP__

#include "geom.hpp"
#include "game.hpp"
#include "renderers.hpp"
#include "serialization.hpp"

#include <cstddef>
#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/unordered_set.hpp>
#include <ostream>
#ifndef NO_GRAPHICS
#include <OpenGLES/ES1/gl.h>
#include "drawing.hpp"
#endif

namespace battlemints {

struct sphere;
struct line;
struct point;

typedef int symbol;

struct thing : boost::noncopyable {
    enum flag_values {
        // collision
        COLLISION_MASK = 0xFF,
        SPHERE = 1, LINE = 4, POINT = 0x10, NO_COLLISION = 0x40,
        SPHERE_SPHERE = 3, SPHERE_LINE = 9, SPHERE_POINT = 0x21,
        LINE_SPHERE = 6, POINT_SPHERE = 0x12,
        // collision layers
        LAYER_MASK = 0xFF00,
        ALL_LAYERS = 0xFF00,
        LAYER_0 = 0x100,
        LAYER_1 = 0x200,
        LAYER_2 = 0x400,
        LAYER_3 = 0x800,
        // misc
        DOES_TICKS = 0x10000,
        CAN_OVERLAP = 0x20000,
        MOVES = 0x40000,
        // thing kinds
        DURIAN = 0x40000000,
        PLAYER = 0x80000000
    };
    
    const int flags;
    vec2 velocity;
    vec2 center;
    symbol label;
    vec2 prev_center;

    thing(vec2 ct, int f)
        : flags(f), velocity(ZERO_VEC2), center(ct), label(0), prev_center(ct) { }

    bool does_collisions() const { return flags != NO_COLLISION; }

    virtual ~thing() { }
    virtual renders_with_range renders_with() const
        { return renderer::null_range; }

    bool does_ticks() const { return flags & DOES_TICKS; }
    bool can_overlap() const { return flags & CAN_OVERLAP; }

    virtual void tick() { }

    bool can_collide_with(thing const &o) const
        { return ((flags | o.flags) & MOVES) && (flags & o.flags & LAYER_MASK); }
    int collision_type(thing const &o) const { return (flags | (o.flags<<1)) & COLLISION_MASK; }

    /* handle physical results of collision */
    void collide(thing &o);

    // returns collision time in s0
    void collision_time_vfp2_r(thing const &o) const;

    /* thing-specific reaction to collision */
    virtual void on_collision(thing &o) { }

    virtual void wall_damage() { }
    virtual void post_damage() { }

    virtual char const * kind() const { return "thing"; }
    virtual void print(std::ostream &os) const
        { os << kind() << " " << (void*)this
             << " v:" << velocity << " c:" << center << " l:" << label
             << " f:" << std::hex << flags; }

    virtual void awaken() { } // Called when board activates

    virtual void trigger(thing *scapegoat) { } // Called when a switch or alarm with the same label is fired

    template<typename T>
    static thing *from_bin(FILE *bin)
    {
        return new T(bin);
    }

#ifndef NO_GRAPHICS
    virtual void draw_self() const { } // only used if renders_with() self_renderer
protected:
    void _push_translate()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(center.x, center.y, 0.0f);
    }

    thing(int f)
        : flags(f), velocity(ZERO_VEC2), center(ZERO_VEC2), label(0), prev_center(ZERO_VEC2)
        {}

    thing(int f, FILE *bin)
        : flags(f), velocity(ZERO_VEC2)
        { BATTLEMINTS_READ_SLOTS(*this, center, label, bin); prev_center = center; }
#endif
};

static inline std::ostream &operator<<(std::ostream &os, thing const &th)
    { th.print(os); return os; }

struct sphere : thing {
    static const float EXHAUST_FACTOR;

    float mass, radius, face_radius, bounce, damp;
    vec2 cur_accel;

    sphere(vec2 ct, float m, float r, float b, float d)
        : thing(ct, SPHERE | DOES_TICKS | MOVES), mass(m), radius(r), bounce(b), damp(d),
          cur_accel(ZERO_VEC2) { }

    sphere(vec2 ct, float m, float r, float b, float d, int flags)
        : thing(ct, SPHERE | DOES_TICKS | MOVES | flags), mass(m), radius(r), bounce(b), damp(d),
          cur_accel(ZERO_VEC2) { }

    virtual char const * kind() const { return "sphere"; }

    virtual vec4 sphere_color(float radius) { return CONST_VEC4_SPLAT(0.0f); }

#ifndef NO_GRAPHICS
    void accelerate_with_exhaust(vec2 accel);

protected:
    sphere(int flags, FILE *bin, float m, float r, float b, float d)
        : thing(SPHERE | DOES_TICKS | MOVES | flags, bin), mass(m),
          radius(r), face_radius(r), bounce(b), damp(d), cur_accel(ZERO_VEC2) {}
#endif
};

struct line : thing {
    vec2 endpoint_a, endpoint_b, normal;

    line(vec2 pt_a, vec2 pt_b)
        : thing((pt_a+pt_b)*0.5f, LINE),
          endpoint_a(pt_a), endpoint_b(pt_b), normal(vperp(vnormalize(endpoint_b - endpoint_a)))
        { }

    line(vec2 pt_a, vec2 pt_b, int flags)
        : thing((pt_a+pt_b)*0.5f, LINE | flags),
          endpoint_a(pt_a), endpoint_b(pt_b), normal(vperp(vnormalize(endpoint_b - endpoint_a)))
        { }

    virtual char const * kind() const { return "line"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " a:" << endpoint_a << " b:" << endpoint_b << " n:" << normal; }

#ifndef NO_GRAPHICS
protected:
    line(int flags, FILE *bin)
        : thing(LINE | flags, bin)
    {
        BATTLEMINTS_READ_SLOTS(*this, endpoint_a, endpoint_b, bin);
        normal = vperp(vnormalize(endpoint_b - endpoint_a));
    }
#endif
};

struct point : thing {
    point(vec2 pt) : thing(pt, POINT) { }

    point(vec2 pt, int flags) : thing(pt, POINT | flags) { } 

    virtual char const * kind() const { return "point"; }

#ifndef NO_GRAPHICS
protected:
    point(int flags, FILE *bin) : thing(POINT | flags, bin) {}
#endif
};

}

#endif
