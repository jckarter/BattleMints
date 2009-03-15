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

boost::array< std::pair<std::string, sign::signface>, 8 > signface_names_array = {{
    std::make_pair("mini_xing",  sign::mini_xing ),
    std::make_pair("mega_xing",  sign::mega_xing ),
    std::make_pair("narrows",    sign::narrows   ),
    std::make_pair("spikes",     sign::spikes    ),
    std::make_pair("slow",       sign::slow      ),
    std::make_pair("stop",       sign::stop      ),
    std::make_pair("leftarrow",  sign::leftarrow ),
    std::make_pair("rightarrow", sign::rightarrow)
}};

}

std::map<std::string, sign::signface> sign::signface_names(signface_names_array.begin(), signface_names_array.end());

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
