#ifndef __EXHAUST_HPP__
#define __EXHAUST_HPP__

#include "thing.hpp"
#include "board.hpp"
#include <deque>
#include <vector>

namespace battlemints {

const unsigned EXHAUST_LIFE_EXPECTANCY = 60;
const unsigned EXHAUST_VERTICES = 4;
extern const float EXHAUST_LIFE_EXPECTANCY_INV;

struct exhaust : thing {
    static const boost::array<vec2, EXHAUST_VERTICES> base_vertices;

    struct particle {
        vec2 center;
        vec2 direction;
        unsigned long birthdate;

        particle() {}
        particle(vec2 c, vec2 d)
            : center(c), direction(d), birthdate(board::current()->tick_count())
            { }

        bool operator==(particle const &p) const
            { return center == p.center && direction == p.direction; }
        bool operator<(particle const &p) const
            { return center == p.center ? direction < p.direction : center < p.center; }
    };

    rect bounding_box;
    std::deque<particle> particles;

    exhaust(rect bb) : thing(0.0, ZERO_VEC2), bounding_box(bb), particles()
        { _vert.reserve(200); _col.reserve(200); }

    virtual bool does_collisions() const { return false; }

    virtual rect visibility_box() { return bounding_box; }
    virtual void draw();
    virtual void tick();

    virtual char const * kind() const { return "exhaust"; }

    void add_particle(vec2 center, vec2 direction)
        { particles.push_back(particle(center, direction)); }

    static vec4 color(unsigned age);

private:
    std::vector<boost::array<vec2, EXHAUST_VERTICES> > _vert;
    std::vector<boost::array<vec4, EXHAUST_VERTICES> > _col;
    
};

}

#endif
