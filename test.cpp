#include "sphere.hpp"
#include "wall.hpp"
#include "wallpost.hpp"
#include <iostream>

// collide [wallpost 0x122d60 v:<0,0> c:<-4.615,-8.46> m:inf] [enemy 0x148ac0 v:<-0.00674599,-0.158967> c:<-4.06236,-8.35077> m:1 color:<1,1,0,1> r:0.563333]
// collide [wall 0x1228c0 v:<0,0> c:<-20.3233,-14.3917> m:inf a:<-16.2867,-14.3917> b:<-24.36,-14.3917> n:<-0,-1>] [enemy 0x1481f0 v:<0.0345902,-0.0202768> c:<-12.3073,-14.314> m:10 color:<1,0,0,1> r:2.55667]

int
main()
{
    using namespace battlemints;

    sphere s(1.0, make_vec2(-12.3073,-14.314), 2.55667, make_vec4(0), 1.0);
    s.velocity = make_vec2(0.0345902,-0.0202768);
    wall w(make_vec2(-16.2867,-14.3917), make_vec2(-24.36,-14.3917));

    std::cout << s.collision_time(w);
    return 0;
}
