#ifndef __COLLISION_HPP__
#define __COLLISION_HPP__

namespace battlemints {

struct sphere;
struct line;
struct point;

void collide_sphere_sphere(sphere &a, sphere &b);
void collide_sphere_line(sphere &a, line &b);
void collide_sphere_point(sphere &a, point &b);
void collide_line_line(line &a, line &b);
void collide_line_point(line &a, point &b);
void collide_point_point(point &a, point &b);

float collision_time_sphere_sphere(sphere const &a, sphere const &b);
float collision_time_sphere_line(sphere const &a, line const &b);
float collision_time_sphere_point(sphere const &a, point const &b);
float collision_time_line_line(line const &a, line const &b);
float collision_time_line_point(line const &a, point const &b);
float collision_time_point_point(point const &a, point const &b);

}

#endif
