#include "player.hpp"
#include "serialization.hpp"
#include "geom.hpp"

namespace battlemints {

void player::tick()
{
    vec2 accel = vclip(controller_state, 1.0) * make_vec2(PLAYER_ACCEL_SCALE);
    accelerate_with_exhaust(accel);
}

thing * player::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);

    return new player(center);
}

}
