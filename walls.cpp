#include "walls.hpp"

namespace battlemints {

boost::array<renders_with_pair, 1> door::renders_with_pairs;

void global_start_walls()
{
    door::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { self_renderer::instance, (renderer_parameter)"wall" } 
    }};
}

}
