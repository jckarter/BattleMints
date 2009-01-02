#include "thing.hpp"
#ifndef NO_GRAPHICS
# include "drawing.hpp"
# include "board.hpp"
#endif
#include "game.hpp"
#include "particles.hpp"
#include <cmath>

namespace battlemints {

rect sphere::visibility_box()
{
    vec2 diagonal = make_vec2(radius);
    return make_rect(center - diagonal, center + diagonal);
}

rect sphere::collision_box()
{
    vec2 diagonal = make_vec2(radius);
    vec2 neg = vmin(ZERO_VEC2, velocity);
    vec2 pos = vmax(ZERO_VEC2, velocity);

    return make_rect(center - diagonal + neg, center + diagonal + pos);
}

#ifndef NO_GRAPHICS
void sphere::accelerate_with_exhaust(vec2 accel)
{
    velocity += accel;
    board::current()->particles_thing()
        ->add_particles(center - vnormalize(accel)*radius, mass*accel, particles::exhaust);
}
#endif

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