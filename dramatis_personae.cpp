#include "dramatis_personae.hpp"
#include "serialization.hpp"
#include "board.hpp"
#include "renderers.hpp"

namespace battlemints {

boost::array<renders_with_pair, 2> player::renders_with_pairs;
boost::array<renders_with_pair, 1> powerup::renders_with_pairs;
boost::array<renders_with_pair, 2> mini::renders_with_pairs;
boost::array<renders_with_pair, 2> mega::renders_with_pairs;
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
        { sphere_renderer::instance, renderer::as_parameter<face_renderer::face_id>(face_renderer::MINI_FACE) }
    }};

    mega::renders_with_pairs = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(mega::RADIUS) },
        { sphere_renderer::instance, renderer::as_parameter<face_renderer::face_id>(face_renderer::MEGA_FACE) }
    }};

    bumper::renders_with_pairs_template = (boost::array<renders_with_pair,2>){{
        { sphere_renderer::instance, renderer::as_parameter<float>(bumper::RADIUS) },
        { sphere_renderer::instance, renderer::as_parameter<float>(bumper::INNER_RADIUS) }
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
