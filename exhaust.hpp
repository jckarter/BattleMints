#ifndef __EXHAUST_HPP__
#define __EXHAUST_HPP__

#include "thing.hpp"

namespace battlemints {

struct exhaust : thing {
    static const boost::array<float, 8> vertices;

    vec2 direction;
    unsigned age;

    exhaust(vec2 ct, vec2 d) : thing(0.0, ct), direction(d), age(0) { }

    virtual bool does_ticks() const { return true; }
    virtual bool does_collisions() const { return false; }

    virtual rect visibility_box();
    virtual void draw();
    virtual void tick();

    virtual char const * kind() const { return "exhaust"; }

private:
    static vec4 _color(unsigned age);
};

}

#endif
