#ifndef __TRIPWIRE_HPP__
#define __TRIPWIRE_HPP__

#include "thing.hpp"
#include "board.hpp"
#include <OpenGLES/ES1/gl.h>

namespace battlemints {

struct tripwire : line {
    tripwire(vec2 pt_a, vec2 pt_b, flag_values flags)
        : line(pt_a, pt_b, CAN_OVERLAP | flags) { }

    /* no physical collision */
    virtual void collide(thing &o) { }
    virtual void collide_sphere(sphere &o) { }
    virtual void collide_line(line &o) { }

    virtual void on_collision(thing &o) {
        if (can_trip(o)) {
            on_trip(o);
        }
    }

    virtual bool can_trip(thing &o) = 0;
    virtual void on_trip(thing &o) = 0;

    virtual char const * kind() const { return "tripwire"; }

protected:
    tripwire(int flags, FILE *bin) : line(CAN_OVERLAP | flags, bin) { }
};

struct goal : tripwire {
    static boost::array<renders_with_pair, 1> renders_with_pairs;

    std::string next_board;
    float trip_strength;

    goal(vec2 pt_a, vec2 pt_b, std::string const &nb)
        : tripwire(pt_a, pt_b, DOES_TICKS), next_board(nb), trip_strength(0.0f) { _set_up_vertices(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual void draw_self() const;
    virtual void tick();
    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual char const * kind() const { return "goal"; }

    static void global_start();
    static void global_finish();

    goal(FILE *bin)
        : tripwire(DOES_TICKS, bin), next_board(*pascal_string_from_bin(bin)), trip_strength(0.0f)
        { _set_up_vertices(); }

    virtual void print(std::ostream &os) const
        { tripwire::print(os); os << " next-board:" << next_board; }

private:
    void _set_up_vertices();
    boost::array<vec2, 4> _vertices;
    boost::array<vec2, 4> _texcoords;

    vec4 _color() const;

    static GLuint _goal_texture;
};

struct alarm : tripwire {
    bool multiple;

    alarm(vec2 pt_a, vec2 pt_b) : tripwire(pt_a, pt_b, DOES_TICKS), multiple(false) { }

    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual char const * kind() const { return "alarm"; }

    alarm(FILE *bin) : tripwire(DOES_TICKS, bin), multiple(false) { }
};

}

#endif
