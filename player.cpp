#include "player.hpp"
#include "serialization.hpp"
#include "geom.hpp"

namespace battlemints {

const float player::ACCEL_SCALE = 0.02f;
const float player::RADIUS = 0.5f;
const vec4 player::COLOR = make_vec4(0.5, 0.89, 1.0, 1.0);
sphere_texture *player::texture = NULL;
sphere_face *player::face = NULL;

inline vec2 player::_cur_accel()
{
    return vclip(controller_state, 1.0) * make_vec2(ACCEL_SCALE);
}

void player::draw()
{
    _push_translate();
    texture->draw();
    glColor4f(0.0, 0.0, 0.0, 1.0);
    face->draw_for_course(velocity, _cur_accel());
    glPopMatrix();
}

void player::tick()
{
    accelerate_with_exhaust(_cur_accel());
}

thing * player::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);

    return new player(center);
}

void player::global_start()
{
    texture = new sphere_texture(RADIUS, COLOR);
    face = sphere_face::from_file_set("player");
}

void player::global_finish()
{
    delete face;
    delete texture;
    texture = NULL; face = NULL;
}

}
