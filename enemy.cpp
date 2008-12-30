#include "enemy.hpp"
#include "mini.hpp"
#include "mega.hpp"
#include "serialization.hpp"
#include "board.hpp"

namespace battlemints {

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

const float mini::ACCEL = 0.025;
const float mini::RADIUS = 0.35;
boost::ptr_vector<sphere_texture> mini::textures(6);
sphere_face *mini::face = NULL;

void mini::global_start()
{
    face = sphere_face::from_file_set("mini");
    textures.push_back(new sphere_texture(RADIUS, make_vec4(1.0, 0.0,  0.0, 1.0)));
    textures.push_back(new sphere_texture(RADIUS, make_vec4(1.0, 0.4,  0.0, 1.0)));
    textures.push_back(new sphere_texture(RADIUS, make_vec4(1.0, 1.0,  0.0, 1.0)));
    textures.push_back(new sphere_texture(RADIUS, make_vec4(0.0, 1.0,  0.0, 1.0)));
    textures.push_back(new sphere_texture(RADIUS, make_vec4(0.3, 0.3,  1.0, 1.0)));
    textures.push_back(new sphere_texture(RADIUS, make_vec4(0.3, 0.24, 0.2, 1.0)));
}

void mini::global_finish()
{
    textures.clear();
    delete face;
}

const float mega::ACCEL = 0.006;
const float mega::RADIUS = 2.6;
sphere_texture *mega::texture = NULL;

void mega::global_start()
{
    texture = new sphere_texture(RADIUS, make_vec4(0.33, 0.13, 0.0, 1.0));
}

void mega::global_finish()
{
    delete texture;
}

}
