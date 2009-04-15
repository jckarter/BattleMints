#include "walls.hpp"
#include "dramatis_personae.hpp"

namespace battlemints {

boost::array<renders_with_pair, 1> door::renders_with_pairs;

void global_start_walls()
{
    door::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { self_renderer::instance, (renderer_parameter)"wall" } 
    }};
}

// arm
void door::_reset_clump(unsigned i)
{
    float clump_life = rand_between(3.0f, 20.0f) * (float)i;
    clump_velocities[i] = (endpoint_b - endpoint_a)/clump_life; 
    clump_lives[i] = (int)clump_life;
    vertices.clumps[i] = endpoint_a;

}

void door::draw_self() const
{
    static vec4 white = make_vec4(1.0f, 1.0f, 1.0f, 1.0f);

    vec4 color = door_color();
    vec4 clump_color = blend(color, white, 0.5f);

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPointSize(2.0f);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, 2);
    glColor4f(color.x, color.y, color.z, color.w);

    if (board::current()->tick_count() & 1)
        glDrawArrays(GL_LINES, 0, 2);
    glColor4f(clump_color.x, clump_color.y, clump_color.z, clump_color.w);
    glDrawArrays(GL_POINTS, 2, NUM_CLUMPS);
}

// arm
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

// arm
void door::_init_draw()
{
    vertices.beam = (boost::array<vec2, 2>){ endpoint_a, endpoint_b };
    for (unsigned i = 0; i < NUM_CLUMPS; ++i)
        _reset_clump(i);
}

vec4 color_door::door_color() const
{
    return color_switch::colors[switch_number];
}

void color_door::tick()
{
    door::tick();
    if (universe::instance.flipped_color_switches[switch_number])
        board::current()->remove_thing(this);
}

}
