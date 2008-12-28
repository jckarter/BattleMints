#include "player.hpp"
#include "serialization.hpp"
#include "geom.hpp"

namespace battlemints {

const float player::ACCEL_SCALE = 0.02f;
const float player::RADIUS = 0.5f;
const vec4 player::COLOR = make_vec4(0.5, 0.89, 1.0, 1.0);
sphere_texture *player::texture = NULL;

void player::tick()
{
    vec2 accel = vclip(controller_state, 1.0) * make_vec2(ACCEL_SCALE);
    accelerate_with_exhaust(accel);
}

thing * player::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);

    return new player(center);
}

void player::global_start()
{
    texture = new sphere_texture(RADIUS, COLOR);
}

void player::global_finish()
{
    delete texture;
    texture = NULL;
}

}
