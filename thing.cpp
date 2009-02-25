#include "thing.hpp"
#ifndef NO_GRAPHICS
# include "drawing.hpp"
# include "board.hpp"
#endif
#include "game.hpp"
#include "particles.hpp"
#include <cmath>

namespace battlemints {

boost::unordered_set<std::string> interned_symbols;

#ifndef NO_GRAPHICS
void sphere::accelerate_with_exhaust(vec2 accel)
{
    velocity += accel;
    board::current()->particles
        .add_particles(center - vnormalize(accel)*radius, mass*accel, particle_system::exhaust);
}
#endif

}
