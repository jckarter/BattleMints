#include "dramatis_personae.hpp"
#include "serialization.hpp"
#include "board.hpp"
#include "renderers.hpp"

namespace battlemints {

boost::array<renders_with_pair, 2> player::renders_with_pairs;
boost::array<renders_with_pair, 1> powerup::renders_with_pairs;
boost::array<renders_with_pair, 2> mini::renders_with_pairs;
boost::array<renders_with_pair, 2> mega::renders_with_pairs;
boost::array<renders_with_pair, 2> switch_spring::renders_with_pairs;
boost::array<renders_with_pair, 2> bumper::renders_with_pairs_template;

void global_start_actors()
{
    player::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(player::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::PLAYER_FACE) }
    }};

    powerup::renders_with_pairs = (boost::array<renders_with_pair,1>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(powerup::RADIUS) }
    }};

    mini::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(mini::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::MINI_FACE) }
    }};

    mega::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(mega::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::MEGA_FACE) }
    }};

    bumper::renders_with_pairs_template = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(bumper::RADIUS) },
        { sphere_renderer::instance, renderer::as_parameter<float>(bumper::INNER_RADIUS) }
    }};

    switch_spring::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { self_renderer::instance,   (renderer_parameter)"switch_spring" },
        { sphere_renderer::instance, renderer::as_parameter<float>(switch_spring::RADIUS) }
    }};
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

void enemy::trigger(thing *scapegoat)
{
    if (!target)
        target = scapegoat;
}

renders_with_range bumper::renders_with() const
{
    return boost::make_iterator_range(
        renders_with_pairs_template.begin() + (board::current()->tick_count() & 1),
        renders_with_pairs_template.end()
    );
}

vec4 bumper::sphere_color(float radius)
{
    if (radius == bumper::RADIUS)
        return bumper::OUTER_COLOR;
    else
        return bumper::INNER_COLOR;
}

void switch_spring::draw_self() const
{
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(SLOT_COLOR.x, SLOT_COLOR.y, SLOT_COLOR.z, SLOT_COLOR.w);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&slot_vertices);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(slot_matrix);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
}

void switch_spring::on_collision(thing &o)
{
    last_touch = &o;
}

void switch_spring::tick()
{
    vec2 perp_axis = vperp(axis);
    vec2 disp = center - home;
    vec2 off_axis = disp * vdot(disp, perp_axis);
    float on_axis = vdot(disp, axis);

    vec2 slot_accel = -off_axis;

    if (on_axis < -SLOT_LENGTH)
        slot_accel += (SLOT_LENGTH + on_axis)*axis;
    else if (on_axis > SLOT_LENGTH)
        slot_accel -= (on_axis - SLOT_LENGTH)*axis;
    else
        slot_accel += axis * signum(on_axis) * (SLOT_LENGTH-fabsf(on_axis)) * SPRING_FACTOR;
    velocity = axis * vdot(axis, velocity) + slot_accel;

    if (!triggered && on_axis > (0.95f * SLOT_LENGTH)) {
        triggered = true;
        if (label)
            board::current()->fire_trigger(label, last_touch);
    }
}

thing *switch_spring::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);
    vec2 axis = vec2_from_json(v["axis"]);
    return new switch_spring(center, axis);
}

void switch_spring::_set_matrix()
{
    vec2 perp_axis = vperp(axis);

    memset(slot_matrix, 0, 16*sizeof(float));
    slot_matrix[ 0] = axis.x;
    slot_matrix[ 1] = axis.y;
    slot_matrix[ 4] = perp_axis.x;
    slot_matrix[ 5] = perp_axis.y;
    slot_matrix[10] = 1.0f;
    slot_matrix[12] = home.x;
    slot_matrix[13] = home.y;
    slot_matrix[15] = 1.0f;
}

}
