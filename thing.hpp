#ifndef __THING_HPP__
#define __THING_HPP__

#include "geom.hpp"
#include "game.hpp"
#include "collision.hpp"
#include "renderers.hpp"
#include "serialization.hpp"

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

typedef std::string const *symbol;

extern boost::unordered_set<std::string> interned_symbols;

inline symbol intern(std::string const &s) { return &*(interned_symbols.insert(s).first); }

struct thing : boost::noncopyable {
    enum flag_values {
        SPHERE = 1, LINE = 4, POINT = 16,
        SPHERE_SPHERE = 3, SPHERE_LINE = 9, SPHERE_POINT = 31,
        LINE_SPHERE = 6, POINT_SPHERE = 18,
        NO_COLLISION = (1<<30)
    };
    
    vec2 velocity;
    vec2 center;
    vec2 prev_center;
    float spring;
    float mass;
    const flag_values flags;
    symbol label;

    thing(float m, vec2 ct, float sp, flag_values f)
        : velocity(ZERO_VEC2), center(ct), prev_center(ct), spring(sp), mass(m),
          flags(f), label(NULL)
        { }

    bool does_collisions() const { return flags != NO_COLLISION; }

    virtual ~thing() { }
    virtual renders_with_range renders_with() const
        { return renderer::null_range; }

    virtual bool does_ticks() const { return false; }
    virtual bool can_overlap() const { return false; }

    virtual void tick() { }

    /* handle physical results of collision */
    void collide(thing &o);

    float collision_time(thing const &o) const;

    /* thing-specific reaction to collision */
    virtual void on_collision(thing &o) { }

    virtual void wall_damage() { }
    virtual void post_damage() { }

    virtual char const * kind() const { return "thing"; }
    virtual void print(std::ostream &os) const
        { os << kind() << " " << (void*)this
             << " v:" << velocity << " c:" << center << " m:" << mass; }

    virtual void awaken() { } // Called when board activates

    virtual void trigger(thing *scapegoat) { } // Called when a switch or alarm with the same label is fired

#ifndef NO_GRAPHICS
    virtual void draw_self() const { } // only used if renders_with() self_renderer
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
    static const float EXHAUST_FACTOR;

    float radius;
    vec2 cur_accel;

    sphere(float m, vec2 ct, float r, float sp)
        : thing(m, ct, sp, SPHERE), radius(r), cur_accel(ZERO_VEC2) { }

    virtual char const * kind() const { return "sphere"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " r:" << radius; }

    virtual bool does_ticks() const { return true; }

    virtual vec4 sphere_color(float radius) { return CONST_VEC4_SPLAT(0.0f); }

#ifndef NO_GRAPHICS
    void accelerate_with_exhaust(vec2 accel);

protected:
    template<typename Thing>
    static thing *from_json(Json::Value const &v)
    {
        vec2 center = vec2_from_json(v["center"]);
        return new Thing(center);
    }
#endif
};

struct line : thing {
    vec2 endpoint_a, endpoint_b, normal;

    line(vec2 pt_a, vec2 pt_b)
        : thing(INFINITYF, (pt_a+pt_b)/2, 0.0f, LINE),
          endpoint_a(pt_a), endpoint_b(pt_b), normal(vperp(vnormalize(endpoint_b - endpoint_a)))
        { }

    virtual char const * kind() const { return "line"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " a:" << endpoint_a << " b:" << endpoint_b << " n:" << normal; }

#ifndef NO_GRAPHICS
protected:
    template<typename Thing>
    static thing *from_json(Json::Value const &v)
    {
        vec2 endpoint_a = vec2_from_json(v["endpoint_a"]);
        vec2 endpoint_b = vec2_from_json(v["endpoint_b"]);

        return new Thing(endpoint_a, endpoint_b);
    }
#endif
};

struct point : thing {
    point (vec2 pt) : thing(INFINITYF, pt, 0.0f, POINT) { }

    virtual char const * kind() const { return "point"; }

#ifndef NO_GRAPHICS
protected:
    template<typename Thing>
    static thing *from_json(Json::Value const &v)
    {
        vec2 center = vec2_from_json(v["center"]);
        return new Thing(center);
    }
#endif
};

}

#endif
