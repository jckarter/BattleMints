#include "collision.hpp"
#include "thing.hpp"
#include "game.hpp"
#include <iostream>

namespace battlemints {

void transfer_momentum(sphere const &a, sphere const &b, vec2 &direction, float &a_coef, float &b_coef)
{
    direction = vnormalize(b.center - a.center);
    float a_comp = vdot(direction, a.velocity);
    float b_comp = vdot(direction, b.velocity);
    float mass_diff = a.mass - b.mass;
    float mass_sum_inv = 1.0f/(a.mass + b.mass);
    
    a_coef = ((2.0f*b.mass*b_comp + mass_diff*a_comp) * mass_sum_inv - a_comp) - b.bounce*a.damp;
    b_coef = ((2.0f*a.mass*a_comp - mass_diff*b_comp) * mass_sum_inv - b_comp) + a.bounce*b.damp;
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
    // Inputs:
    // {s8-9}:   pt_a
    // {s24-25}: pt_b
    // {s16-7}:  velocity
    //  s1:      radius
    void _collision_time_points_vfp2_r()
    {
#if 0
        vec2 distance = pt_a - pt_b;
        float distance2 = vnorm2(distance);
        float radius2 = radius*radius;
        float spread = vdot(velocity, distance);

        if (spread > 0.0 && distance2 <= radius2)
            return 0.0;

        float speed2 = vnorm2(velocity);

        return (spread - sqrtf(spread*spread - speed2*(distance2 - radius2))) / speed2;
#else
        __asm__ volatile (
            "fsubs   s8, s8, s24\n\t" // {s8-9} = distance = pt_a - pt_b
            "fmuls   s24, s8, s8\n\t"
            "fadds   s0, s24, s25\n\t" // s0 = distance2 = vnorm2(pt_a - pt_b)
            "fnmacs  s0, s1, s1\n\t" // s0 = distance2 - radius*radius
            "fmuls   s24, s8, s16\n\t"
            "fadds   s2, s24, s25\n\t" // s2 = spread = vdot(velocity, distance)
            "fcmpzs  s2\n\t"
            "fmstat\n\t"
            "ble     1f\n\t"
            "fcmpzs  s0\n\t"
            "fmstat\n\t"
            "bhi     1f\n\t"
            "fsubs   s0, s0, s0\n\t"
            "b       2f\n\t"
            "1:\n\t"
            "fmuls   s16, s16, s16\n\t"
            "fadds   s3, s16, s17\n\t" // s3 = speed2 = vnorm2(velocity)
            "fmuls   s4, s2, s2\n\t"
            "fnmacs  s4, s0, s3\n\t"
            "fsqrts  s4, s4\n\t" // s4 = sqrtf(spread*spread - speed2*(distance2 - radius2))
            "fsubs   s0, s2, s4\n\t"
            "fdivs   s0, s0, s3\n\t"

            "2:\n\t"
            : : : "cc"
        );
#endif
    }
}

void collision_time_sphere_line_vfp2_r(sphere const &a, line const &b)
{
#if 0
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
        "fldd    d4, [%[a_velocity]]\n\t" // {s8-9} = a.velocity
        "fldd    d12, [%[b_normal]]\n\t" // {s24-5} = b.normal
        "fmuls   s16, s8, s24\n\t"
        "fadds   s0, s16, s17\n\t" // s0 = side = vdot(a.velocity, b.normal) = 1
        "fcmpzs  s0\n\t"
        "fmstat\n\t" // cc = signum(side) = !lo
        "flds    s0, [%[a_radius]]\n\t"
        "fmuls   s24, s24, s0\n\t"
        "fnegslo s24, s24\n\t" // {s24-5} = normal = signum(side) * a.radius * b.normal
        "fldd    d8, [%[a_center]]\n\t"
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
        "fmsr    s0, %[infinity]\n\t"
        "b       2f\n\t"

        "1:\n\t"
        // return result
        "fmuls   s12, s12, s24\n\t"
        "fadds   s0, s12, s13\n\t"
        "fmuls   s8, s8, s24\n\t"
        "fadds   s1, s8, s9\n\t"
        "fdivs   s0, s0, s1\n\t" // s0 = result = vdot(dist_a, normal)/vdot(a.velocity, normal)

        "2:\n\t"
        :
        : [a_velocity] "r" (&a.velocity),
          [a_center] "r" (&a.center),
          [a_radius] "r" (&a.radius),
          [b_endpoint_a] "r" (&b.endpoint_a),
          [b_endpoint_b] "r" (&b.endpoint_b),
          [b_normal] "r" (&b.normal),
          [infinity] "r" (INFINITYF)
        : "cc"
    );
#endif
}

void collision_time_sphere_point_vfp2_r(sphere const &a, point const &b)
{
    // {s8-9}:   pt_a
    // {s24-25}: pt_b
    // {s16-7}:  velocity
    //  s1:      radius
    __asm__ volatile (
        "fldd d4, [%[b_center]]\n\t"
        "fldd d12, [%[a_center]]\n\t"
        "fldd d8, [%[a_velocity]]\n\t"
        "flds s1, [%[a_radius]]\n\t"
        :
        : [b_center] "r" (&b.center),
          [a_center] "r" (&a.center),
          [a_velocity] "r" (&a.velocity),
          [a_radius] "r" (&a.radius)
    );
    _collision_time_points_vfp2_r();
}

void collision_time_sphere_sphere_vfp2_r(sphere const &a, sphere const &b)
{
    __asm__ volatile (
        "fldd  d4, [%[a_center]]\n\t"
        "fldd  d12, [%[b_center]]\n\t"
        "fldd  d8, [%[b_velocity]]\n\t"
        "fldd  d13, [%[a_velocity]]\n\t"
        "fsubs s16, s16, s26\n\t"
        "flds  s1, [%[a_radius]]\n\t"
        "flds  s2, [%[b_radius]]\n\t"
        "fadds s1, s1, s2\n\t"
        :
        : [a_center] "r" (&a.center),
          [b_center] "r" (&b.center),
          [b_velocity] "r" (&b.velocity),
          [a_velocity] "r" (&a.velocity),
          [a_radius] "r" (&a.radius),
          [b_radius] "r" (&b.radius)
    );
    _collision_time_points_vfp2_r();
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

void thing::collision_time_vfp2_r(thing const &o) const
{
    switch ((flags | (o.flags<<1)) & 0xFF) {
    case SPHERE_SPHERE:
        collision_time_sphere_sphere_vfp2_r(*static_cast<sphere const *>(this), *static_cast<sphere const *>(&o));
        break;
    case SPHERE_LINE:
        collision_time_sphere_line_vfp2_r(*static_cast<sphere const *>(this), *static_cast<line const *>(&o));
        break;
    case SPHERE_POINT:
        collision_time_sphere_point_vfp2_r(*static_cast<sphere const *>(this), *static_cast<point const *>(&o));
        break;
    case LINE_SPHERE:
        collision_time_sphere_line_vfp2_r(*static_cast<sphere const *>(&o), *static_cast<line const *>(this));
        break;
    case POINT_SPHERE:
        collision_time_sphere_point_vfp2_r(*static_cast<sphere const *>(&o), *static_cast<point const *>(this));
        break;
    default:
        vfp_set_s0(INFINITYF);
    }
}

}
