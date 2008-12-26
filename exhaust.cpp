#include "exhaust.hpp"
#include "board.hpp"
#include <boost/array.hpp>
#include <boost/static_assert.hpp>

namespace battlemints {

const float EXHAUST_LIFE_EXPECTANCY_INV = 1.0f/(float)EXHAUST_LIFE_EXPECTANCY;

const boost::array<vec2, EXHAUST_VERTICES> exhaust::base_vertices = {
     make_vec2( 0.0,   3.0),
     make_vec2(-9.0,  24.0),
     make_vec2( 0.0,  -3.0),
     make_vec2(-9.0, -24.0)
};

void exhaust::draw()
{
    unsigned num_particles = particles.size();
    unsigned num_vertices = EXHAUST_VERTICES * num_particles;

    _vert.resize(num_particles);
    _col.resize(num_particles);

    std::deque<particle>::iterator pi;
    std::vector<boost::array<vec2, EXHAUST_VERTICES> >::iterator vi;
    std::vector<boost::array<vec4, EXHAUST_VERTICES> >::iterator ci;

    vec4 colr;
    float agef;
    unsigned long tick = 0;
    for (pi = particles.begin(), vi = _vert.begin(), ci = _col.begin();
         pi != particles.end();
         ++pi, ++vi, ++ci) {

        if (tick != pi->birthdate) {
            tick = pi->birthdate;
            unsigned long age = board::current()->tick_count() - pi->birthdate;
            agef = (float)age; colr = color(age);
        }

        vec2 dir = pi->direction * agef * EXHAUST_LIFE_EXPECTANCY_INV;
        vec2 normal = vperp(dir);
        for (unsigned n = 0; n < EXHAUST_VERTICES; ++n) {
            (*vi)[n] = pi->center + base_vertices[n].x * dir + base_vertices[n].y * normal;
            (*ci)[n] = colr;
        }
    }

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, (void*)&_vert[0]);
    glColorPointer(4, GL_FLOAT, 0, (void*)&_col[0]);
    glDrawArrays(GL_LINES, 0, num_vertices);

    glDisableClientState(GL_COLOR_ARRAY);
}

void exhaust::tick()
{
    unsigned long death_tick = board::current()->tick_count() < EXHAUST_LIFE_EXPECTANCY
        ? 0 : board::current()->tick_count() - EXHAUST_LIFE_EXPECTANCY;
    
    while (!particles.empty() && particles.front().birthdate <= death_tick)
        particles.pop_front();
}

vec4 exhaust::color(unsigned age)
{
    float a = 1.0 - (float)age/(float)EXHAUST_LIFE_EXPECTANCY;
    float a2 = sqrtf(a);

    return make_vec4(a2, (a+a2)*0.5, a, a*a);
}

}
