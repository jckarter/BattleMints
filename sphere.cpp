#include "sphere.hpp"
#ifndef NO_GRAPHICS
# include "drawing.hpp"
# include "board.hpp"
#endif
#include "game.hpp"
#include "exhaust.hpp"
#include "serialization.hpp"
#include "sound_effects.hpp"
#include <cmath>
#include <vector>
#include <boost/cstdint.hpp>

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

void sphere::draw()
{
#ifndef NO_GRAPHICS
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(center.x, center.y, 0.0f);

    texture.draw();

    glPopMatrix();
#endif
}

#ifndef NO_GRAPHICS
void sphere::accelerate_with_exhaust(vec2 accel)
{
    velocity += accel;
    board::current()->exhaust_thing()
        ->add_particle(center - vnormalize(accel)*radius, mass*accel);
}
#endif

thing *sphere::from_json(Json::Value const &v)
{
    vec2  center = vec2_from_json(v["center"]);
    vec4  color  = vec4_from_json(v["color"]);
    float radius = (float)v["radius"].asDouble();
    float mass   = (float)v["mass"].asDouble();
    float spring = (float)v["spring"].asDouble();

    return new sphere(mass, center, radius, color, spring);
}

}
