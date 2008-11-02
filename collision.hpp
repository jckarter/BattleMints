#ifndef __COLLISION_HPP__
#define __COLLISION_HPP__

namespace battlemints {

struct sphere;

void collide_sphere_sphere(sphere &a, sphere &b);

float collision_time_sphere_sphere(sphere const &a, sphere const &b);

}

#endif
