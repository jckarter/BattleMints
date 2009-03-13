#include "dramatis_personae.hpp"
#include "serialization.hpp"
#include "board.hpp"
#include "renderers.hpp"

namespace battlemints {

boost::array<renders_with_pair, 3> player::renders_with_pairs_template;
boost::array<renders_with_pair, 3> player::invuln_renders_with_pairs;
boost::array<renders_with_pair, 1> powerup::renders_with_pairs;
boost::array<renders_with_pair, 2> mini::renders_with_pairs;
boost::array<renders_with_pair, 2> mega::renders_with_pairs;
boost::array<renders_with_pair, 2> switch_spring::renders_with_pairs;
boost::array<renders_with_pair, 2> bumper::renders_with_pairs_template;
boost::array<renders_with_pair, 1> pellet::renders_with_pairs;

void global_start_actors()
{
    player::renders_with_pairs_template = (boost::array<renders_with_pair,3>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(player::SHIELD_RADIUS) },
        { sphere_renderer::instance, renderer::as_parameter<float>(player::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::PLAYER_FACE) }
    }};

    player::invuln_renders_with_pairs = (boost::array<renders_with_pair,3>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(player::SHIELD_RADIUS) },
        { sphere_renderer::instance, renderer::as_parameter<float>(player::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::PLAYER_INVULN_FACE) }
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

    pellet::renders_with_pairs = (boost::array<renders_with_pair,1>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(pellet::RADIUS) }
    }};
}

renders_with_range player::renders_with() const
{
    return invuln
        ? boost::make_iterator_range(invuln_renders_with_pairs.begin(), invuln_renders_with_pairs.end())
        : boost::make_iterator_range(
              renders_with_pairs_template.begin() + (!shielded || (board::current()->tick_count() & 1)),
              renders_with_pairs_template.end()
          );
}

void player::tick()
{
    if (grace_period > 0)
        --grace_period;
    cur_accel = vclip(controller_state, 1.0) * make_vec2(ACCEL_SCALE); 

    if (cur_accel != ZERO_VEC2)
        accelerate_with_exhaust(cur_accel);

    if (pellet_burn > 0) {
        --pellet_burn;
        if (pellet_burn == 0) {
            pellet_burn = INVULN_PELLET_BURN;
            --pellets;
        }
    }

    if (invuln && pellets == 0)
        lose_invuln();
}

void player::update_stats()
{
    if (invuln) {
        radius = SHIELD_RADIUS;
        bounce = INVULN_SPRING;
        mass   = INVULN_MASS;
        damp   = INVULN_DAMP;
    } else if (shielded) {
        radius = SHIELD_RADIUS;
        bounce = SHIELD_SPRING;
        mass   = MASS;
        damp   = DAMP;
    } else {
        radius = RADIUS;
        bounce = SPRING;
        mass   = MASS;
        damp   = DAMP;
    }
}

vec4 player::sphere_color(float r)
{
    return invuln
        ? (r == SHIELD_RADIUS ? invuln_colors[board::current()->tick_count() % invuln_colors.size()] : INVULN_BODY_COLOR)
        : (r == SHIELD_RADIUS ? SHIELD_COLOR : COLOR);
}

void player::lose_shield()
{
    shielded = false;
    board::current()->particles.explode(this, false);
    grace_period = GRACE_PERIOD;
    update_stats();
}

void player::gain_shield()
{
    shielded = true;
    update_stats();
}

void player::lose_invuln()
{
    invuln = false;
    update_stats();
}

void player::gain_invuln()
{
    invuln = true;
    update_stats();
    if (pellets < 20)
        pellets = 20;
    pellet_burn = INVULN_PELLET_BURN;
}

void player::die()
{
    board::current()->particles.explode(this, true);
    board::restart_with<death_transition>();
}

void player::damage()
{
    if (!invuln) {
        if (shielded)
            lose_shield();
        else if (grace_period == 0)
            die();
    }
}

vec4 powerup::sphere_color(float)
{
    if (charge_time > 0)
        return DEAD_COLOR;
    else {
        return blend(
            CHARGED_COLOR, pulse_colors[powerup_kind],
            fast_sin_2pi((float)(board::current()->tick_count() & 63) * (1.0f/64.0f))
        );
    }
}

void powerup::tick()
{
    if (charge_time > 0)
        --charge_time;
}

void powerup::give_powerup(player *p)
{
    switch (powerup_kind) {
    case shield:
        p->gain_shield();
        charge_time = CHARGE_TIME;
        break;
    case invuln:
        p->gain_invuln();
        board::current()->particles.explode(this, true);
        break;
    }
}

void powerup::on_collision(thing &o)
{
    if (charge_time == 0) {
        if (o.flags & PLAYER) {
            give_powerup(static_cast<player*>(&o));
        }
    }
}

void pellet::on_collision(thing &o)
{ 
    if (o.flags & PLAYER) {
        player *p = static_cast<player*>(&o);
        ++p->pellets;
        board::current()->remove_thing(this);
    }
}

namespace {

boost::array< std::pair<std::string, powerup::kind_name>, 2 > kind_names_array = {{
    std::make_pair("shield", powerup::shield),
    std::make_pair("invuln", powerup::invuln)
}};

}

std::map<std::string, powerup::kind_name> powerup::kind_names(kind_names_array.begin(), kind_names_array.end());

thing* powerup::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);
    powerup::kind_name powerup_kind = kind_names[v["kind"].asString()];
    return new powerup(center, powerup_kind);
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

    float axis_pos = vdot(disp, axis);
    float spring = 0.0f;

    if (axis_pos < -SLOT_LENGTH)
        spring = (-SLOT_LENGTH - axis_pos);
    else if (axis_pos > SLOT_LENGTH)
        spring = ( SLOT_LENGTH - axis_pos);

    spring += signum(axis_pos) * (SLOT_LENGTH - fabsf(axis_pos)) * SPRING_FACTOR;

    vec2 axis_return = -perp_axis * vdot(disp + velocity, perp_axis);
    velocity += axis_return + spring * axis;

    if (!triggered && axis_pos > (0.95f * SLOT_LENGTH)) {
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
