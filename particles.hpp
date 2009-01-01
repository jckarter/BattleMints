#ifndef __EXHAUST_HPP__
#define __EXHAUST_HPP__

#include "thing.hpp"
#include "board.hpp"
#include <boost/circular_buffer.hpp>
#include <vector>

namespace battlemints {

struct particles : thing {
    static const unsigned LIFE_EXPECTANCY = 60;
    static const unsigned PARTICLES = 1600;
    static const float LIFE_EXPECTANCY_INV;

    static const boost::array<vec2, 4> exhaust;

    rect bounding_box;

    particles(rect bb)
        : thing(0.0f, ZERO_VEC2, 1.0f), bounding_box(bb),
          _vertices(PARTICLES), _deltas(PARTICLES), _colors(PARTICLES), _ages(PARTICLES)
        { }

    virtual bool does_collisions() const { return false; }

    virtual rect visibility_box() { return bounding_box; }
    virtual void draw();
    virtual void tick();

    virtual char const * kind() const { return "particles"; }

    static vec4 color(unsigned age)
    {
        float a = 1.0 - (float)age/(float)LIFE_EXPECTANCY;
        float a2 = sqrtf(a);

        return make_vec4(a2, (a+a2)*0.5, a, a*a);
    }

    template<typename Container>
    void add_particles(vec2 center, vec2 direction, Container const &particles)
    {
        vec2 dir = direction * LIFE_EXPECTANCY_INV;
        vec2 normal = vperp(dir);
        vec4 co = color(0);

        BOOST_FOREACH(vec2 particle, particles) {
            _vertices.push_back(center);
            _deltas.push_back(particle.x * dir + particle.y * normal);
            _colors.push_back(co);
            _ages.push_back(0);
        }
    }


private:
    typedef boost::circular_buffer<vec2> vertex_buffer;
    typedef boost::circular_buffer<vec4> color_buffer;
    typedef boost::circular_buffer<unsigned> age_buffer;

    vertex_buffer _vertices, _deltas;
    color_buffer _colors;
    age_buffer _ages;
};

}

#endif
