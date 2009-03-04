#ifndef __TRIPWIRE_HPP__
#define __TRIPWIRE_HPP__

#include "thing.hpp"
#include "board.hpp"
#include <OpenGLES/ES1/gl.h>

namespace battlemints {

struct tripwire : line {
    tripwire(vec2 pt_a, vec2 pt_b)
        : line(pt_a, pt_b) { }

    /* no physical collision */
    virtual void collide(thing &o) { }
    virtual void collide_sphere(sphere &o) { }
    virtual void collide_line(line &o) { }

    virtual bool can_overlap() const { return true; }

    virtual void on_collision(thing &o) {
        if (can_trip(o)) {
            on_trip(o);
        }
    }

    virtual bool can_trip(thing &o) = 0;
    virtual void on_trip(thing &o) = 0;

    virtual char const * kind() const { return "tripwire"; }
};

struct goal : tripwire {
    static boost::array<renders_with_pair, 1> renders_with_pairs;

    std::string next_board;
    float trip_strength;

    goal(vec2 pt_a, vec2 pt_b, std::string const &nb)
        : tripwire(pt_a, pt_b), next_board(nb), trip_strength(0.0f) { _set_up_vertices(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual void draw_self() const;
    virtual void tick();
    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual char const * kind() const { return "goal"; }

    static thing *from_json(Json::Value const &v);
    static void global_start();
    static void global_finish();

private:
    void _set_up_vertices();
    boost::array<vec2, 4> _vertices;
    boost::array<vec2, 4> _texcoords;

    vec4 _color() const;

    static GLuint _goal_texture;
};

struct alarm : tripwire {
    bool multiple;

    alarm(vec2 pt_a, vec2 pt_b) : tripwire(pt_a, pt_b), multiple(false) { }
        
    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual char const * kind() const { return "alarm"; }

    static thing *from_json(Json::Value const &v) { return line::from_json<alarm>(v); }
};

}

#endif
