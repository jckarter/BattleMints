#ifndef __COLLISION_HPP__
#define __COLLISION_HPP__

namespace battlemints {

struct sphere;
struct wall;

void collide_sphere_sphere(sphere &a, sphere &b);
void collide_sphere_wall(sphere &a, wall &b);
void collide_wall_wall(wall &a, wall &b);

float collision_time_sphere_sphere(sphere const &a, sphere const &b);
float collision_time_sphere_wall(sphere const &a, wall const &b);
float collision_time_wall_wall(wall const &a, wall const &b);

}

#endif
