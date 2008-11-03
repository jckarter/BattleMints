#ifndef __COLLISION_HPP__
#define __COLLISION_HPP__

namespace battlemints {

struct sphere;
struct spring;

void collide_sphere_sphere(sphere &a, sphere &b);
void collide_sphere_spring(sphere &a, spring &b);

float collision_time_sphere_sphere(sphere const &a, sphere const &b);

}

#endif
