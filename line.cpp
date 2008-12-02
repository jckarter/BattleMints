#include "line.hpp"

namespace battlemints {

rect
line::visibility_box()
{
    return make_rect(vmin(endpoint_a, endpoint_b), vmax(endpoint_a, endpoint_b));
}

rect
line::collision_box()
{
    return make_rect(vmin(endpoint_a, endpoint_b), vmax(endpoint_a, endpoint_b));
}


}
