#ifndef __COLLISION_HPP__
#define __COLLISION_HPP__

namespace battlemints {

struct sphere;
struct line;
struct point;

void collide_sphere_sphere(sphere &a, sphere &b);
void collide_sphere_line(sphere &a, line &b);
void collide_sphere_point(sphere &a, point &b);

// these all return collision time in s0
void collision_time_sphere_sphere_vfp2_r(sphere const &a, sphere const &b);
void collision_time_sphere_line_vfp2_r(sphere const &a, line const &b);
void collision_time_sphere_point_vfp2_r(sphere const &a, point const &b);

}

#endif
