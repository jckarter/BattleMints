#include "collision.hpp"
#include "thing.hpp"
#include "sphere.hpp"
#include "spring.hpp"
#include <iostream>

namespace battlemints {

void transfer_momentum(thing const &a, thing const &b, vec2 &direction, float &a_coef, float &b_coef)
{
    direction = vnormalize(b.center - a.center);
    float a_comp = vdot(direction, a.velocity);
    float b_comp = vdot(direction, b.velocity);
    float mass_diff = a.mass - b.mass;
    float mass_sum_inv = 1/(a.mass + b.mass);
    
    a_coef = (2*b.mass*b_comp + mass_diff*a_comp) * mass_sum_inv - a_comp;
    b_coef = (2*a.mass*a_comp - mass_diff*b_comp) * mass_sum_inv - b_comp;

}

void collide_sphere_sphere(sphere &a, sphere &b)
{
    vec2 direction;
    float a_coef, b_coef;
    transfer_momentum(a, b, direction, a_coef, b_coef);

    a.velocity += a_coef*direction;
    b.velocity += b_coef*direction;
}

void collide_sphere_spring(sphere &a, spring &b)
{
    vec2 direction;
    float a_coef, b_coef;
    transfer_momentum(a, b, direction, a_coef, b_coef);

    a.velocity += 5*a_coef*direction;
    b.velocity += b_coef*direction;
}

float collision_time_sphere_sphere(sphere const &a, sphere const &b)
{
    vec2 distance = a.center - b.center;
    vec2 velocity = b.velocity - a.velocity;
    float radius = a.radius + b.radius;
    float speed2 = vnorm2(velocity);
    float distance2 = vnorm2(distance);
    float spread = vdot(velocity, distance);

    return (spread - sqrtf(spread*spread - speed2*(distance2 - radius*radius))) / speed2;
}

}
