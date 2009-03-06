#include "walls.hpp"

namespace battlemints {

boost::array<renders_with_pair, 1> door::renders_with_pairs;

void global_start_walls()
{
    door::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { self_renderer::instance, (renderer_parameter)"wall" } 
    }};
}

void door::_reset_clump(unsigned i)
{
    float clump_life = rand_between(3.0f, 20.0f) * (float)i;
    clump_velocities[i] = (endpoint_b - endpoint_a)/clump_life; 
    clump_lives[i] = (int)clump_life;
    vertices.clumps[i] = endpoint_a;

}

void door::draw_self() const
{
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPointSize(4.0f);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, 2);
    glColor4f(COLOR.x, COLOR.y, COLOR.z, COLOR.w);

    if (board::current()->tick_count() & 1)
        glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_POINTS, 2, NUM_CLUMPS);
}

void door::tick()
{
    for (unsigned i = 0; i < NUM_CLUMPS; ++i)
        if (--clump_lives[i] == 0)
            _reset_clump(i);

    float *vi = (float*)&vertices.clumps, *di = (float*)&clump_velocities;
    unsigned count = NUM_CLUMPS * 2;
    for (; count > 0; --count) // XXX vectorize
        *vi++ += *di++;
}

door::door(vec2 pt_a, vec2 pt_b)
    : wall(pt_a, pt_b)
{
    vertices.beam = (boost::array<vec2, 2>){ pt_a, pt_b };
    for (unsigned i = 0; i < NUM_CLUMPS; ++i)
        _reset_clump(i);
}

}
