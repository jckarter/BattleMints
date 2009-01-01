#include "thing.hpp"
#include "walls.hpp"
#include <iostream>

// [wall a:<-24.315,-12.0967> b:<-16.3117,-12.06> n:<-0.00458128,0.99999>]
// [wall a:<-16.2867,-14.3917> b:<-24.36,-14.3917> n:<-0,-1>]
// [enemy 0x1481c0 v:<0.00539702,0.00232456> c:<-13.3376,-13.3188> r:2.55667]

int
main()
{
    using namespace battlemints;

    sphere s(1.0, make_vec2(-13.3376,-13.3188), 2.55667, make_vec4(0), 1.0);
    s.velocity = make_vec2(0.00539702,0.00232456);

    wall w1(make_vec2(-24.315,-12.0967),  make_vec2(-16.3117,-12.06));
    wall w2(make_vec2(-16.2867,-14.3917), make_vec2(-24.36,-14.3917));

    std::cout << s.collision_time(w1) << "\n";
    std::cout << s.collision_time(w2) << "\n";
    return 0;
}
