#ifndef __EXHAUST_HPP__
#define __EXHAUST_HPP__

#include <boost/circular_buffer.hpp>
#include <vector>
#include "geom.hpp"

namespace battlemints {

struct thing;

struct particle_system {
    static const unsigned LIFE_EXPECTANCY = 60;
    static const unsigned PARTICLES = 2000;
    static const float LIFE_EXPECTANCY_INV;

    static const boost::array<vec2, 4> exhaust;

    particle_system()
        : _vertices(PARTICLES), _deltas(PARTICLES), _colors(PARTICLES), _ages(PARTICLES)
        { }

    void draw();
    void tick();

    void explode(thing *th);

    static vec4 color(unsigned age)
    {
        float a = 1.0f - (float)age/(float)LIFE_EXPECTANCY;
        float a2 = sqrtf(a);

        return make_vec4(a2, (a+a2)*0.5f, a, a*a);
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
