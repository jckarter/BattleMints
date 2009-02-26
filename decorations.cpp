#include "decorations.hpp"

namespace battlemints {

const boost::array<vec2, 4> sign::vertices = {
    make_vec2(-0.7, -0.7),
    make_vec2( 0.7, -0.7),
    make_vec2(-0.7,  0.7),
    make_vec2( 0.7,  0.7)
};

boost::array<renders_with_pair, 1> sign::renders_with_pairs;

namespace {

boost::array< std::pair<std::string, signface>, x > signface_names_array = {{
    { "mini_xing", sign::mini_xing },
    { "mega_xing", sign::mega_xing },
    { "narrows",   sign::narrows   },
    { "spikes",    sign::spikes    },
    { "blank",     sign::blank     },
}};

}

std::map<std::string, signface> sign::signface_names(signface_names_array.begin(), signface_names_array.end());

void global_start_decorations()
{
    sign::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { decoration_renderer::instance, decoration_renderer::SIGN_DECORATION }
    }};
}

decoration::params sign::decoration_params()
{
    return (params){
        (void*)vertices,
        (void*)texcoords
    };
}

}
