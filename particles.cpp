#include "particles.hpp"
#include "board.hpp"
#include <boost/array.hpp>
#include <boost/static_assert.hpp>

namespace battlemints {

const float particles::LIFE_EXPECTANCY_INV = 1.0f/(float)particles::LIFE_EXPECTANCY;

const boost::array<vec2, 4> particles::exhaust = {
    make_vec2( 0.0,   3.0),
    make_vec2(-9.0,  24.0),
    make_vec2( 0.0,  -3.0),
    make_vec2(-9.0, -24.0)
};

void particles::explode(thing *th)
{
    static const unsigned SHOCKWAVE_PARTICLES = 50, DEBRIS_PARTICLES = 50;

    if (board::current()->thing_lives(th)) {
        boost::array<vec2, 2*(SHOCKWAVE_PARTICLES + DEBRIS_PARTICLES)> explosion;
        unsigned i = 0;
        while (i < 2*SHOCKWAVE_PARTICLES) {
            float rho = rand_between(64.0, 81.0);
            float theta = rand_between(0.0, 1.0);
            explosion[i++] = polar_vec2(rho, rand_near(theta, 0.0625));
            explosion[i++] = polar_vec2(rho, rand_near(theta, 0.0625));
        }

        while (i < 2*(SHOCKWAVE_PARTICLES + DEBRIS_PARTICLES)) {
            float rho = rand_between(3.0, 7.0);
            float theta = rand_between(0.0, 1.0);
            explosion[i++] = polar_vec2(rand_near(rho*rho, 3.0), theta);
            explosion[i++] = polar_vec2(rand_near(rho*rho, 3.0), theta);
        }

        add_particles(th->center, make_vec2(1.0, 0.0), explosion);

        board::current()->remove_thing(th);
    }
}

void particles::draw()
{
#if 1
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    vertex_buffer::array_range vertices_one = _vertices.array_one(), vertices_two = _vertices.array_two();

    if (vertices_one.second > 0) {
        glVertexPointer(2, GL_FLOAT, 0, (void*)vertices_one.first);
        glColorPointer(4, GL_FLOAT, 0, (void*)_colors.array_one().first);
        glDrawArrays(GL_LINES, 0, vertices_one.second);
    }

    if (vertices_two.second > 0) {
        glVertexPointer(2, GL_FLOAT, 0, (void*)vertices_two.first);
        glColorPointer(4, GL_FLOAT, 0, (void*)_colors.array_two().first);
        glDrawArrays(GL_LINES, 0, vertices_two.second);
    }

    glDisableClientState(GL_COLOR_ARRAY);
#endif
}

static inline void _add_vertex_array_ranges(particles::vertex_buffer::array_range const &v, particles::vertex_buffer::array_range const &d)
{
    float *vi = (float*)v.first, *di = (float*)d.first;
    unsigned count = v.second * 2;
    for (; count > 0; --count) // XXX vectorize
        *vi++ += *di++;
}

void particles::tick()
{
    if (_ages.empty())
        return;

    while (_ages.front() >= LIFE_EXPECTANCY) {
        _vertices.pop_front();
        _deltas.pop_front();
        _colors.pop_front();
        _ages.pop_front();
    }

    _add_vertex_array_ranges(_vertices.array_one(), _deltas.array_one());
    _add_vertex_array_ranges(_vertices.array_two(), _deltas.array_two());

    color_buffer::iterator ci = _colors.begin();
    age_buffer::iterator ai = _ages.begin();
    for (; ci != _colors.end(); ++ci, ++ai) {
        ++*ai; 
        *ci = color(*ai);
    }
}

}
