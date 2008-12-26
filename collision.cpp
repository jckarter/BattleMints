#include "collision.hpp"
#include "thing.hpp"
#include "sphere.hpp"
#include "line.hpp"
#include <iostream>
#include <limits>

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

// lines and points don't move
void collide_line_line(line &a, line &b) { }
void collide_line_point(line &a, thing &b) { }
void collide_point_point(thing &a, thing &b) { }

float collision_time_line_line(line const &a, line const &b) { return INFINITYF; }
float collision_time_line_point(line const &a, thing const &b) { return INFINITYF; }
float collision_time_point_point(thing const &a, thing const &b) { return INFINITYF; }

void collide_sphere_line(sphere &a, line &b)
{
    a.velocity = vreflect(b.normal, a.velocity);
}

void collide_sphere_point(sphere &a, thing &b)
{
    a.velocity = vreflect(vnormalize(a.center - b.center), a.velocity);
}

void collide_sphere_sphere(sphere &a, sphere &b)
{
    vec2 direction;
    float a_coef, b_coef;
    transfer_momentum(a, b, direction, a_coef, b_coef);

    a.velocity += a_coef*direction;
    b.velocity += b_coef*direction;
}

static inline float _collision_time_points(vec2 pt_a, vec2 pt_b, vec2 velocity, float radius)
{
    vec2 distance = pt_a - pt_b;
    float distance2 = vnorm2(distance);
    float radius2 = radius*radius;
    float spread = vdot(velocity, distance);

    if (spread > 0.0 && distance2 <= radius2)
        return 0.0;

    float speed2 = vnorm2(velocity);

    return (spread - sqrtf(spread*spread - speed2*(distance2 - radius2))) / speed2;
}

static inline float _collision_time_point_line(vec2 pt, vec2 line_pt, vec2 normal, vec2 velocity)
{
    return vdot(line_pt - pt, normal)/vdot(velocity, normal);
}

float collision_time_sphere_line(sphere const &a, line const &b)
{
    float side = signum(vdot(a.velocity, b.normal));

    vec2 normal = side * b.normal * a.radius;
    vec2 near_pt = a.center + normal;

    vec2 vel_perp = side * vperp(a.velocity);
    float cos_a = vdot((b.endpoint_a - near_pt), vel_perp);
    float cos_b = vdot((b.endpoint_b - near_pt), vel_perp);

    if (cos_a >= 0.0 && cos_b <= 0.0)
        return /*(vdot(b.endpoint_a - near_pt, normal) * vdot(b.endpoint_a - a.center, normal) < 0)
            ? 0.0 :*/ _collision_time_point_line(near_pt, b.endpoint_a, b.normal, a.velocity);
    else
        return std::numeric_limits<float>::infinity();
}

float collision_time_sphere_point(sphere const &a, thing const &b)
{
    return _collision_time_points(b.center, a.center, a.velocity, a.radius);
}

float collision_time_sphere_sphere(sphere const &a, sphere const &b)
{
    return _collision_time_points(
        a.center, b.center, b.velocity - a.velocity, a.radius + b.radius
    );
}

}
