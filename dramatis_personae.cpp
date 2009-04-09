#include "dramatis_personae.hpp"
#include "serialization.hpp"
#include "board.hpp"
#include "renderers.hpp"
#include "universe.hpp"
#include <unistd.h>

namespace battlemints {

boost::array<renders_with_pair, 3> player::renders_with_pairs_template;
boost::array<renders_with_pair, 3> player::invuln_renders_with_pairs;
boost::array<renders_with_pair, 1> powerup::renders_with_pairs;
boost::array<renders_with_pair, 2> mini::renders_with_pairs;
boost::array<renders_with_pair, 3> shield_mini::renders_with_pairs;
boost::array<renders_with_pair, 2> bomb::renders_with_pairs;
boost::array<renders_with_pair, 2> durian::renders_with_pairs;
boost::array<renders_with_pair, 2> mega::renders_with_pairs;
boost::array<renders_with_pair, 2> trigger_switch::renders_with_pairs;
boost::array<renders_with_pair, 2> heavy_switch::renders_with_pairs;
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

    shield_mini::renders_with_pairs = (boost::array<renders_with_pair,3>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(shield_mini::OUTER_RADIUS) },
        { sphere_renderer::instance, renderer::as_parameter<float>(mini::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::MINI_FACE) }
    }};

    bomb::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(mini::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::MINI_WHITE_FACE) }
    }};

    mega::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(mega::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::MEGA_FACE) }
    }};

    durian::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { spike_renderer::instance, renderer::as_parameter<float>(durian::RADIUS) },
        { face_renderer::instance,   renderer::as_parameter<face_renderer::face_id>(face_renderer::MINI_FACE) }
    }};

    bumper::renders_with_pairs_template = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(bumper::RADIUS) },
        { sphere_renderer::instance, renderer::as_parameter<float>(bumper::INNER_RADIUS) }
    }};

    trigger_switch::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { self_renderer::instance,   (renderer_parameter)"trigger_switch" },
        { sphere_renderer::instance, renderer::as_parameter<float>(trigger_switch::RADIUS) }
    }};

    heavy_switch::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { self_renderer::instance,   (renderer_parameter)"heavy_switch" },
        { sphere_renderer::instance, renderer::as_parameter<float>(heavy_switch::RADIUS) }
    }};

    pellet::renders_with_pairs = (boost::array<renders_with_pair,1>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(pellet::RADIUS) }
    }};
}

renders_with_range shield_mini::renders_with() const
{
    return boost::make_iterator_range(
        renders_with_pairs.begin() + (!shielded || (board::current()->tick_count() & 1)),
        renders_with_pairs.end()
    );
}

void shield_mini::damage()
{
    if (shielded)
        lose_shield();
    else
        die();
}

void shield_mini::lose_shield()
{
    shielded = false;
    radius = mini::RADIUS;
    bounce = mini::SPRING;
}

void bomb::die()
{
    if (explode_time < 0) {
        explode_time = EXPLODE_TIME;
        radius = EXPLOSION_RADIUS;
        face_radius = EXPLOSION_RADIUS;
        bounce = EXPLOSION_SPRING;
        mass = EXPLOSION_MASS;
    }
}

void bomb::tick()
{
    enemy::tick();
    if (explode_time > 0) {
        board::current()->particles.explode(this, false);
        --explode_time;
    }
    else if (explode_time == 0) {
        board::current()->remove_thing(this);
    }
    else if (target) {
        if (fuse > 0) {
            --fuse;
            if (fuse == 0)
                die();
        }
    } else
        fuse = FUSE;
}

vec4 bomb::sphere_color(float r)
{
    int fuse_time = FUSE - fuse;
    return target
        ? blend(COLOR, BLUSH_COLOR, fast_sin_2pi(fuse_time*fuse_time*BLUSH_FACTOR))
        : COLOR;
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
    if (pellet_grace_period > 0)
        --pellet_grace_period;
    cur_accel = vclip(controller_state, 1.0) * make_vec2(ACCEL_SCALE); 

    if (cur_accel != ZERO_VEC2)
        accelerate_with_exhaust(cur_accel);

    if (invuln && pellet_burn > 0) {
        --pellet_burn;
        if (pellet_burn == 0) {
            pellet_burn = INVULN_PELLET_BURN;
            --pellets;
        }
    }

    if (panic_charge > 0)
        --panic_charge;
    if (panicked > 0) {
        --panicked;
        if (panicked == 0)
            update_stats();
    }

    if (!invuln && controller_tap_count >= PANIC_TAP_COUNT && panic_charge == 0) {
        panic();
    }

    if (invuln && pellets == 0)
        lose_invuln();
}

void player::awaken()
{
    board::current()->player_thing = this;
}

void player::panic()
{
    panicked = PANIC_TIME;
    panic_charge = PANIC_CHARGE;
    update_stats();
}

void player::update_stats()
{
    if (invuln) {
        radius = SHIELD_RADIUS;
        face_radius = SHIELD_RADIUS;
        bounce = INVULN_SPRING;
        mass   = INVULN_MASS;
        damp   = INVULN_DAMP;
    } else {
        face_radius = RADIUS;
        if (shielded) {
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

        if (panicked > 0) {
            radius = PANIC_RADIUS;
            bounce = PANIC_SPRING;
            mass   = PANIC_MASS;
        }
    }
}

vec4 player::sphere_color(float r)
{
    if (invuln)
        return (r == SHIELD_RADIUS ? invuln_colors[board::current()->tick_count() % invuln_colors.size()] : INVULN_BODY_COLOR);
    else if (panicked > 0)
        return (r == SHIELD_RADIUS ? SHIELD_COLOR : PANIC_COLOR);
    else
        return (r == SHIELD_RADIUS ? SHIELD_COLOR : COLOR);
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
    pellets += 20;
    pellet_burn = INVULN_PELLET_BURN;
}

void player::die()
{
    board::current()->particles.explode(this, true);
    board::restart_with<death_transition>();
}

void player::lose_pellets()
{
    int spill = pellets < MAX_PELLET_SPILL ? pellets : MAX_PELLET_SPILL;
    pellets = 0;
    grace_period = GRACE_PERIOD;
    pellet_grace_period = GRACE_PERIOD*2;
    board::current()->particles.explode(this, false);

    for (int i = 0; i < spill; ++i)
        loose_pellet::spawn(*this);
}

void player::damage()
{
    if (!(board::current()->flags & board::SAFE) && !invuln && !panicked) {
        if (shielded)
            lose_shield();
        else if (grace_period == 0) {
            if (pellets > 0)
                lose_pellets();
            else 
                die();
        }
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

void loose_pellet::tick()
{
    --lifetime;
    if (lifetime == 0)
        board::current()->remove_thing(this);
}

void loose_pellet::on_collision(thing &o)
{
    if (o.flags & PLAYER) {
        player *p = static_cast<player*>(&o);
        if (p->pellet_grace_period == 0) {
            ++p->pellets;
            board::current()->remove_thing(this);
        }
    }
}

// arm
loose_pellet *loose_pellet::spawn(sphere const &from)
{
    float speed = rand_between(MIN_SPEED, MAX_SPEED);
    float theta = rand_between(0.0f, 1.0f);
    float initial_radius = from.radius + pellet::RADIUS;

    loose_pellet *lp = new loose_pellet(polar_vec2(initial_radius, theta) + from.center);
    lp->velocity = polar_vec2(speed, theta);

    board::current()->add_thing(lp);

    return lp;
}

// arm
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
    if (!target && (o.flags & MOVES))
        target = &o;
}

void enemy::trigger(thing *scapegoat)
{
    if (!target)
        target = scapegoat;
}

void durian::on_collision(thing &o)
{
    enemy::on_collision(o);

    if (stuck_to)
        stuck_to = NULL;

    if ((o.flags & LINE) || (o.flags & POINT))
        stick_to(o);
    else if (o.flags & DURIAN) {
        die();
        static_cast<durian*>(&o)->die();
    } else
        o.post_damage();
}

void durian::tick()
{
    enemy::tick();

    if (stuck_to) {
        if (board::current()->thing_lives(stuck_to))
            velocity = ZERO_VEC2;
        else
            stuck_to = NULL;
    }
}

void durian::stick_to(thing &o)
{
    stuck_to = &o;
    velocity = ZERO_VEC2;
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

void switch_base::draw_self() const
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

void switch_base::on_collision(thing &o)
{
    last_touch = &o;
}

// arm
void switch_base::tick()
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
        switch_on();
    }
}

// arm
void switch_base::_set_matrix()
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

void trigger_switch::switch_on()
{
    if (label)
        board::current()->fire_trigger(label, last_touch);
}

void
eraser_switch::switch_on()
{
    unlink(universe::filename(universe_name).c_str());
    if (label)
        board::current()->fire_trigger(label, last_touch);
}

}
