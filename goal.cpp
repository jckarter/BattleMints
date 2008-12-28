#include "goal.hpp"
#include "serialization.hpp"
#include "player.hpp"
#include "transition.hpp"
#include <cstdio>

namespace battlemints {

static const float GOAL_THICKNESS = 0.1f;
static const float GOAL_TRIP_STRENGTH_FALLOFF = 0.85f;

GLuint goal::_goal_texture;

inline vec4
goal::_color()
{
    return make_vec4(1.0f - trip_strength, 1.0f - 0.5f*trip_strength*trip_strength, 1.0f - trip_strength, 1.0f);
}

void
goal::draw()
{
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindTexture(GL_TEXTURE_2D, _goal_texture);

    vec4 col = _color();
    glColor4f(col.x, col.y, col.z, col.w);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&_vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&_texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void
goal::tick()
{
    trip_strength *= GOAL_TRIP_STRENGTH_FALLOFF;
}

void
goal::on_trip(thing &o)
{
    trip_strength = 1.0f;
    board::change_board_with<goal_transition>(next_board);
}

bool
goal::can_trip(thing &o)
{
    return dynamic_cast<player*>(&o) != NULL;
}

thing *
goal::from_json(Json::Value const &v)
{
    vec2 endpoint_a = vec2_from_json(v["endpoint_a"]);
    vec2 endpoint_b = vec2_from_json(v["endpoint_b"]);
    std::string next_board = v["next_board"].asString();

    return new goal(endpoint_a, endpoint_b, next_board);
}

void
goal::global_start()
{
    boost::array<unsigned char, 16> checkerboard_texture = {
        0,   0,   255, 255,
        0,   0,   255, 255,
        255, 255, 0,   0,
        255, 255, 0,   0
    };

    glGenTextures(1, &_goal_texture);
    glBindTexture(GL_TEXTURE_2D, _goal_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
        4, 4, 0,
        GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*)&checkerboard_texture
    );
}

void
goal::global_finish()
{
    glDeleteTextures(1, &_goal_texture);
}

void
goal::_set_up_vertices()
{
    float texture_length = vnorm(endpoint_b - endpoint_a)/(GOAL_THICKNESS*2.0f);

    _vertices[0] = endpoint_a - GOAL_THICKNESS*normal;
    _vertices[1] = endpoint_b - GOAL_THICKNESS*normal;
    _vertices[2] = endpoint_a + GOAL_THICKNESS*normal;
    _vertices[3] = endpoint_b + GOAL_THICKNESS*normal;

    _texcoords[0] = make_vec2(0.0f,           0.0f);
    _texcoords[1] = make_vec2(texture_length, 0.0f);
    _texcoords[2] = make_vec2(0.0f,           1.0f);
    _texcoords[3] = make_vec2(texture_length, 1.0f);
}

}
