#include "decorations.hpp"

namespace battlemints {

const boost::array<vec2, 4> sign::vertices = {
    make_vec2(-0.7, -0.7),
    make_vec2( 0.7, -0.7),
    make_vec2(-0.7,  0.7),
    make_vec2( 0.7,  0.7)
};

boost::array<renders_with_pair, 1> sign::renders_with_pairs;

void global_start_decorations()
{
    sign::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { decoration_renderer::instance, decoration_renderer::SIGN_DECORATION }
    }};
}

decoration::params sign::decoration_params()
{
    return (params){ (void*)&vertices, (void*)&texcoords };
}

}
