#include "collision.hpp"
#include "thing.hpp"
#include "game.hpp"
#include <iostream>

namespace battlemints {

void transfer_momentum(thing const &a, thing const &b, vec2 &direction, float &a_coef, float &b_coef)
{
    direction = vnormalize(b.center - a.center);
    float a_comp = vdot(direction, a.velocity);
    float b_comp = vdot(direction, b.velocity);
    float mass_diff = a.mass - b.mass;
    float mass_sum_inv = 1.0f/(a.mass + b.mass);
    
    a_coef = ((2.0f*b.mass*b_comp + mass_diff*a_comp) * mass_sum_inv - a_comp) - b.spring/sqrtf(a.mass);
    b_coef = ((2.0f*a.mass*a_comp - mass_diff*b_comp) * mass_sum_inv - b_comp) + a.spring/sqrtf(b.mass);
}

void collide_sphere_line(sphere &a, line &b)
{
    a.velocity = vreflect(b.normal, a.velocity);
}

void collide_sphere_point(sphere &a, point &b)
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

namespace {
    float _collision_time_points(vec2 const &pt_a, vec2 const &pt_b, vec2 const &velocity, float radius)
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
}

float collision_time_sphere_line(sphere const &a, line const &b)
{
#ifndef __arm__
    float side = signum(vdot(a.velocity, b.normal));

    vec2 normal = side * b.normal * a.radius;
    vec2 near_pt = a.center + normal;

    vec2 vel_perp = side * vperp(a.velocity);
    vec2 dist_a = b.endpoint_a - near_pt;
    vec2 dist_b = b.endpoint_b - near_pt;
    float cos_a = vdot(dist_a, vel_perp);
    float cos_b = vdot(dist_b, vel_perp);

    if (cos_a >= 0.0 && cos_b <= 0.0)
        return vdot(dist_a, normal)/vdot(a.velocity, normal);
    else
        return INFINITYF;
#else
    __asm__ volatile (
        // set vector size to 2
        "fmrx r1, fpscr\n\t"
        "bic  r1, r1, #0x00370000\n\t"
        "orr  r1, r1, #0x00010000\n\t"
        "fmxr fpscr, r1\n\t"

        // do our biz
        "fldmias %[a_velocity], {s8-s9}\n\t" // {s8-9} = a.velocity
        "fldmias %[b_normal], {s24-s25}\n\t" // {s24-5} = b.normal
        "fmuls   s16, s8, s24\n\t"
        "fadds   s0, s16, s17\n\t" // s0 = side = vdot(a.velocity, b.normal) = 1
        "fcmpzs  s0\n\t"
        "fmstat\n\t" // cc = signum(side) = !lo
        "flds    s0, [%[a_radius]]\n\t"
        "fmuls   s24, s24, s0\n\t"
        "fnegslo s24, s24\n\t" // {s24-5} = normal = signum(side) * a.radius * b.normal
        "fldmias %[a_center], {s16-s17}\n\t"
        "fadds   s16, s16, s24\n\t" // {s16-7} = near_pt = a.center + normal
        "fcpys   s5, s8\n\t"
        "fnegs   s4, s9\n\t" // {s4-5} = vel_perp = vperp(a.velocity)

        "fldmias %[b_endpoint_a], {s12-s15}\n\t"
        "fsubs   s12, s12, s16\n\t" // {s12-3} = dist_a = b.endpoint_a - near_pt
        "fmuls   s10, s4, s12\n\t"
        "fsubs   s14, s14, s16\n\t"
        "fmuls   s14, s4, s14\n\t"
        "fadds   s0, s10, s11\n\t" // s0 = cos_a = vdot(dist_a, vel_perp)
        "fadds   s1, s14, s15\n\t" // s1 = cos_b = vdot(dist_b, vel_perp)

        "fmuls   s0, s0, s1\n\t"
        "fcmpzs  s0\n\t"
        "fmstat\n\t"
        "blo     1f\n\t"

        // return infinity
        "mov     r0, #0xFF000000\n\t"
        "mov     r0, r0, lsr #1\n\t"
        "b       2f\n\t"

        "1:\n\t"
        // return result
        "fmuls   s12, s12, s24\n\t"
        "fmuls   s8, s8, s24\n\t"
        "fadds   s0, s12, s13\n\t"
        "fadds   s1, s8, s9\n\t"
        "fdivs   s0, s0, s1\n\t" // s0 = result = vdot(dist_a, normal)/vdot(a.velocity, normal)
        "fmrs    r0, s0\n\t"

        "2:\n\t"
        // reset vector size to 1 (scalar)
        "fmrx r1, fpscr\n\t"
        "bic  r1, r1, #0x00370000\n\t"
        "fmxr fpscr, r1\n\t"
        :
        : [a_velocity] "r" (&a.velocity),
          [a_center] "r" (&a.center),
          [a_radius] "r" (&a.radius),
          [b_endpoint_a] "r" (&b.endpoint_a),
          [b_endpoint_b] "r" (&b.endpoint_b),
          [b_normal] "r" (&b.normal)
        : "r0", "r1", "cc"
    );
#endif
}

float collision_time_sphere_point(sphere const &a, point const &b)
{
    return _collision_time_points(b.center, a.center, a.velocity, a.radius);
}

float collision_time_sphere_sphere(sphere const &a, sphere const &b)
{
    return _collision_time_points(
        a.center, b.center, b.velocity - a.velocity, a.radius + b.radius
    );
}

void thing::collide(thing &o)
{
    switch ((flags | (o.flags<<1)) & 0xFF) {
    case SPHERE_SPHERE:
        collide_sphere_sphere(*static_cast<sphere*>(this), *static_cast<sphere*>(&o));
        break;
    case SPHERE_LINE:
        collide_sphere_line(*static_cast<sphere*>(this), *static_cast<line*>(&o));
        break;
    case SPHERE_POINT:
        collide_sphere_point(*static_cast<sphere*>(this), *static_cast<point*>(&o));
        break;
    case LINE_SPHERE:
        collide_sphere_line(*static_cast<sphere*>(&o), *static_cast<line*>(this));
        break;
    case POINT_SPHERE:
        collide_sphere_point(*static_cast<sphere*>(&o), *static_cast<point*>(this));
        break;
    default:
        break;
    }
}

float thing::collision_time(thing const &o) const
{
    switch ((flags | (o.flags<<1)) & 0xFF) {
    case SPHERE_SPHERE:
        return collision_time_sphere_sphere(*static_cast<sphere const *>(this), *static_cast<sphere const *>(&o));
    case SPHERE_LINE:
        return collision_time_sphere_line(*static_cast<sphere const *>(this), *static_cast<line const *>(&o));
    case SPHERE_POINT:
        return collision_time_sphere_point(*static_cast<sphere const *>(this), *static_cast<point const *>(&o));
    case LINE_SPHERE:
        return collision_time_sphere_line(*static_cast<sphere const *>(&o), *static_cast<line const *>(this));
    case POINT_SPHERE:
        return collision_time_sphere_point(*static_cast<sphere const *>(&o), *static_cast<point const *>(this));
    default:
        return INFINITYF;
    }
}

}
