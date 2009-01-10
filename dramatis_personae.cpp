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
    glMatrixMode(GL_MODELVIEW);
    glScalef(RADIUS, RADIUS, RADIUS);
    face->draw_for_course(velocity, _cur_accel());
    glPopMatrix();
}

void player::tick()
{
    accelerate_with_exhaust(_cur_accel());
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

void mini::draw()
{
    _push_translate();
    glColor4f(color.x, color.y, color.z, color.w);
    texture->draw();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glScalef(RADIUS, RADIUS, RADIUS);
    face->draw_for_course(velocity, cur_accel);
    glPopMatrix();
}

sphere_texture *mega::texture = NULL;
sphere_face *mega::face = NULL;

void mega::global_start()
{
    face = sphere_face::from_file("mega");
    texture = new sphere_texture(RADIUS, COLOR);
}

void mega::global_finish()
{
    delete texture;
}

void mega::draw()
{
    _push_translate();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    texture->draw();
    glScalef(RADIUS, RADIUS, RADIUS);
    face->draw_for_course(velocity, 3.0f*cur_accel);
    glPopMatrix();
}

sphere_texture *bumper::inner_texture = NULL, *bumper::outer_texture = NULL;

void bumper::global_start()
{
    inner_texture = new sphere_texture(INNER_RADIUS, INNER_COLOR);
    outer_texture = new sphere_texture(RADIUS, OUTER_COLOR);
}

void bumper::global_finish()
{
    delete inner_texture; delete outer_texture;
}

void bumper::draw()
{
    _push_translate();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    outer_texture->draw();
    inner_texture->draw();
    glPopMatrix();
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
