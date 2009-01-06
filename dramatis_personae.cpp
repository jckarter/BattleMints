#include "dramatis_personae.hpp"
#include "serialization.hpp"
#include "board.hpp"

namespace battlemints {

void camera::tick()
{
    if (target && board::current()->thing_lives(target)) {
        vec2 goal = target->center + target->velocity * LEAD_FACTOR;
        vec2 goal_velocity = (goal - center) * FOLLOW_FACTOR;
        velocity += ACCEL * vnormalize(goal_velocity - velocity);
    } else
        target = NULL;
}

sphere_texture *player::texture = NULL;
sphere_face *player::face = NULL;

inline vec2 player::_cur_accel()
{
    return vclip(controller_state, 1.0) * make_vec2(ACCEL_SCALE);
}

void player::draw()
{
    _push_translate();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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
    face = sphere_face::from_file("player");
}

void player::global_finish()
{
    delete face;
    delete texture;
    texture = NULL; face = NULL;
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

sphere_texture *mini::texture = NULL;
sphere_face *mini::face = NULL;
boost::array<vec4, 6> mini::colors = {
    make_vec4(1.0, 0.0,  0.0, 1.0),
    make_vec4(1.0, 0.4,  0.0, 1.0),
    make_vec4(1.0, 1.0,  0.0, 1.0),
    make_vec4(0.0, 1.0,  0.0, 1.0),
    make_vec4(0.3, 0.3,  1.0, 1.0),
    make_vec4(0.3, 0.24, 0.2, 1.0)
};

void mini::global_start()
{
    face = sphere_face::from_file("mini");
    texture = new sphere_texture(RADIUS, make_vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void mini::global_finish()
{
    delete texture;
    delete face;
}

sphere_texture *mega::texture = NULL;
sphere_face *mega::face = NULL;

void mega::global_start()
{
    texture = new sphere_texture(RADIUS, COLOR);
    face = sphere_face::from_file("mega");
}

void mega::global_finish()
{
    delete texture;
}

}
