#ifndef __EXHAUST_HPP__
#define __EXHAUST_HPP__

#include "thing.hpp"
#include <vector>

namespace battlemints {

static const unsigned EXHAUST_LIFE_EXPECTANCY = 60;

struct exhaust : thing {
    static const boost::array<vec2, 4> vertices;

    struct particle {
        vec2 center;
        vec2 direction;
        unsigned age;

        particle() {}
        particle(vec2 c, vec2 d) : center(c), direction(d), age(0) { }

        bool operator==(particle const &p) const
            { return center == p.center && direction == p.direction; }
        bool operator<(particle const &p) const
            { return center == p.center ? direction < p.direction : center < p.center; }
    };

    rect bounding_box;
    std::vector<particle> particles;

    exhaust(rect bb) : thing(0.0, ZERO_VEC2), bounding_box(bb), particles() { }

    virtual bool does_collisions() const { return false; }

    virtual rect visibility_box() { return bounding_box; }
    virtual void draw();
    virtual void tick();

    virtual char const * kind() const { return "exhaust"; }

    void add_particle(vec2 center, vec2 direction)
        { particles.push_back(particle(center, direction)); }

    static vec4 color(unsigned age);
};

}

#endif
