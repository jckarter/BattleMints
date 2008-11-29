#include "enemy.hpp"
#include "serialization.hpp"

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
    if (cur_accel != make_vec2(0.0))
        accelerate_with_exhaust(cur_accel);

    if (target) {
        vec2 a = cur_accel, b = accel*vnormalize(target->center - center),
             c = a*(1.0-responsiveness) + b*responsiveness;
        cur_accel = c == make_vec2(0.0)
            ? make_vec2(0.0)
            : accel*vnormalize(c);
    } else {
        cur_accel = make_vec2(0.0);
    }
}

void enemy::on_collision(thing &o)
{
    sphere::on_collision(o);

    if (!target)
        target = &o;
}

}
