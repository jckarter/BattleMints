#include "exhaust.hpp"
#include "board.hpp"
#include <boost/array.hpp>
#include <boost/static_assert.hpp>

namespace battlemints {

const boost::array<vec2, 4> exhaust::vertices = {
     make_vec2( 0.0,   4.0),
     make_vec2(-9.0,  24.0),
     make_vec2( 0.0, - 4.0),
     make_vec2(-9.0, -24.0)
};

void exhaust::draw()
{
    unsigned num_particles = particles.size();
    unsigned num_vertices = 4 * num_particles;

    // XXX move vectors into object to avoid allocation
    std::vector<boost::array<vec2, 4> > vert(num_particles);
    std::vector<boost::array<vec4, 4> > col (num_particles);

    std::vector<particle>::iterator pi;
    std::vector<boost::array<vec2, 4> >::iterator vi;
    std::vector<boost::array<vec4, 4> >::iterator ci;

    for (pi = particles.begin(), vi = vert.begin(), ci = col.begin();
         pi != particles.end();
         ++pi, ++vi, ++ci) {

        vec2 dir = pi->direction * (float)pi->age/(float)EXHAUST_LIFE_EXPECTANCY;
        vec2 normal = vperp(dir);
        vec4 color = _color(pi->age);
        for (unsigned n = 0; n < 4; ++n) {
            (*vi)[n] = pi->center + vertices[n].x * dir + vertices[n].y * normal;
            (*ci)[n] = color;
        }
    }

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, (void*)&vert[0]);
    glColorPointer(4, GL_FLOAT, 0, (void*)&col[0]);
    glDrawArrays(GL_LINES, 0, num_vertices);

    glDisableClientState(GL_COLOR_ARRAY);
}

void exhaust::tick()
{
    for (std::vector<particle>::iterator i = particles.begin(); i != particles.end(); ) {
        ++i->age;
        if (i->age > EXHAUST_LIFE_EXPECTANCY)
            particles.erase(i);
        else
            ++i;
    }
}

vec4 exhaust::_color(unsigned age)
{
    float a = 1.0 - (float)age/(float)EXHAUST_LIFE_EXPECTANCY;
    float a2 = sqrtf(a);

    return make_vec4(a2, (a+a2)*0.5, a, a*a);
}

}
