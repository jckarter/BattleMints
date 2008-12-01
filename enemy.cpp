#include "enemy.hpp"
#include "serialization.hpp"
#include "board.hpp"

namespace battlemints {

thing *enemy::from_json(Json::Value const &v)
{
    vec2  center          = vec2_from_json(v["center"]);
    vec4  color           = vec4_from_json(v["color"]);
    float radius          = (float)v["radius"].asDouble();
    float mass            = (float)v["mass"].asDouble();
    float spring          = (float)v["spring"].asDouble();
    float accel           = (float)v["accel"].asDouble();
    float responsiveness  = (float)v["responsiveness"].asDouble();

    return new enemy(mass, center, radius, color, spring, accel, responsiveness);
}

void enemy::tick()
{
    if (cur_accel != ZERO_VEC2)
        accelerate_with_exhaust(cur_accel);

    if (target) {
        if (board::current()->thing_lives(target)) {
            vec2 a = cur_accel, b = accel*vnormalize(target->center - center),
                 c = a*(1.0-responsiveness) + b*responsiveness;
            cur_accel = c == ZERO_VEC2
                ? ZERO_VEC2
                : accel*vnormalize(c);
        } else {
            target = NULL;
        }
    } else {
        cur_accel = ZERO_VEC2;
    }
}

void enemy::on_collision(thing &o)
{
    if (!target)
        target = &o;
}

}
