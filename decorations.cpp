#include "decorations.hpp"

namespace battlemints {

const boost::array<vec2, 4> sign::vertices = {
    make_vec2(-0.7, -0.7),
    make_vec2( 0.7, -0.7),
    make_vec2(-0.7,  0.7),
    make_vec2( 0.7,  0.7)
};

boost::array<renders_with_pair, 1> sign::renders_with_pairs;
boost::array<renders_with_pair, 2> battlemints_flag::renders_with_pairs;

boost::array<vec2, flag::NUM_VERTICES> flag::texcoords = {
    make_vec2(0.0f,   1.0f),
    make_vec2(0.0f,   0.0f),
    make_vec2(0.125f, 1.0f),
    make_vec2(0.125f, 0.0f),
    make_vec2(0.25f,  1.0f),
    make_vec2(0.25f,  0.0f),
    make_vec2(0.375f, 1.0f),
    make_vec2(0.375f, 0.0f),
    make_vec2(0.5f,   1.0f),
    make_vec2(0.5f,   0.0f),
    make_vec2(0.625f, 1.0f),
    make_vec2(0.625f, 0.0f),
    make_vec2(0.75f,  1.0f),
    make_vec2(0.75f,  0.0f),
    make_vec2(0.875f, 1.0f),
    make_vec2(0.875f, 0.0f),
    make_vec2(1.0f,   1.0f),
    make_vec2(1.0f,   0.0f),
};

boost::array<vec2, 4> flag::flagpost_texcoords = {
    make_vec2(0.0f, 1.0f),
    make_vec2(1.0f, 1.0f),
    make_vec2(0.0f, 0.0f),
    make_vec2(1.0f, 0.0f)
};

boost::array<vec2, 4> flag::flagpost_vertices = {
    make_vec2(-0.1f, 0.0f),
    make_vec2( 0.1f, 0.0f),
    make_vec2(-0.1f, 1.8f),
    make_vec2( 0.1f, 1.8f)
};

void global_start_decorations()
{
    sign::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { decoration_renderer::instance, decoration_renderer::SIGN_DECORATION }
    }};

    battlemints_flag::renders_with_pairs = (boost::array<renders_with_pair, 2>){{
        { decoration_renderer::instance, decoration_renderer::BATTLEMINTS_FLAG_DECORATION },
        { decoration_renderer::instance, decoration_renderer::FLAGPOST_DECORATION }
    }};
}

decoration::params sign::decoration_params(decoration_renderer::decoration_id d)
{
    return (params){ (void*)&vertices, (void*)&texcoords, 4 };
}

void flag::tick()
{
    base_flap.phase += FLAP_PHASE_STEP;

    int i = 0; float theta = 0.0f;
    float height_offset = HEIGHT_OFFSET;
    for (; i < NUM_VERTICES; i += 2, theta += 2.0f/(NUM_VERTICES-2)) {
        float y = base_flap.height(theta);

        if (i == 0)
            height_offset -= 0.5f*y;
        vertices[i  ] = make_vec2(theta * WIDTH, y+height_offset);
        vertices[i+1] = make_vec2(theta * WIDTH, y+HEIGHT+height_offset);
    }
}

decoration::params
flag::decoration_params(decoration_renderer::decoration_id d)
{
    if (d == decoration_renderer::FLAGPOST_DECORATION)
        return (params){ (void*)&flagpost_vertices, (void*)&flagpost_texcoords, 4 };
    else
        return (params){ (void*)&vertices, (void*)&texcoords, NUM_VERTICES };
}

flag::flag(FILE *bin)
    : decoration(DOES_TICKS, bin)
{
    _reset_base_flap(base_flap);
}

void
flag::_reset_flap(flag::flap &f)
{
    f.wavelength = rand_between(FLAP_MIN_WAVELENGTH, FLAP_MAX_WAVELENGTH);
    f.amplitude = rand_between(FLAP_MIN_INITIAL_AMPLITUDE, FLAP_MAX_INITIAL_AMPLITUDE);
    f.phase = rand_between(0.0f, 1.0f);
}

void
flag::_reset_base_flap(flag::flap &f)
{
    f.wavelength = rand_between(BASE_FLAP_MIN_WAVELENGTH, BASE_FLAP_MAX_WAVELENGTH);
    f.amplitude = rand_between(BASE_FLAP_MIN_INITIAL_AMPLITUDE, BASE_FLAP_MAX_INITIAL_AMPLITUDE);
    f.phase = rand_between(0.0f, 1.0f);
}

}
