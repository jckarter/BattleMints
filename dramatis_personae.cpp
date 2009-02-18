#include "dramatis_personae.hpp"
#include "serialization.hpp"
#include "board.hpp"

namespace battlemints {

void global_start_actors()
{
}

void player::tick()
{
    cur_accel = vclip(controller_state, 1.0) * make_vec2(ACCEL_SCALE); 
    if (cur_accel != ZERO_VEC2)
        accelerate_with_exhaust(cur_accel);
}

void powerup::tick()
{
    spin += SPIN;
}

thing* powerup::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);
    std::string kind = v["kind"].asString();
    return new powerup(center, kind);
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

void spring::tick()
{
    velocity += factor * (home - center);
}

void direction_spring::tick()
{
    vec2 distance = home - center;
    float factor = blend(perpendicular_factor, axis_factor, vdot(vnormalize(distance), axis));
    velocity += factor * distance;
}

}
