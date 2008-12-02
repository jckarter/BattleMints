#ifndef __TRIPWIRE_HPP__
#define __TRIPWIRE_HPP__

#include "line.hpp"
#include "board.hpp"

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
//            board::current()->remove_thing(this);
        }
    }

    virtual bool can_trip(thing &o) = 0;
    virtual void on_trip(thing &o) = 0;

    virtual char const * kind() const { return "tripwire"; }
};

}

#endif
