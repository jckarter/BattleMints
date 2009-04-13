#include "tripwire.hpp"
#include "serialization.hpp"
#include "dramatis_personae.hpp"
#include "transition.hpp"
#include "drawing.hpp"
#include "board.hpp"
#include "universe.hpp"
#include "game.hpp"
#include <cstdio>
#include <boost/lexical_cast.hpp>
#include <sys/stat.h>

namespace battlemints {

const float GOAL_THICKNESS = 0.1f;
const float loader::TEXT_SCALE = 1.0f/60.0f;

boost::array<renders_with_pair, 1>
    goal::renders_with_pairs,
    stage_exit::renders_with_pairs,
    world_exit::renders_with_pairs,
    loader::renders_with_pairs;

GLuint goal::_goal_texture;
GLuint goal::_arrow_texture;

void
global_start_tripwires()
{
    goal::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { self_renderer::instance, (renderer_parameter)"goal" }
    }};

    stage_exit::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { self_renderer::instance, (renderer_parameter)"stage_exit" }
    }};

    world_exit::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { self_renderer::instance, (renderer_parameter)"world_exit" }
    }};

    loader::renders_with_pairs = (boost::array<renders_with_pair, 1>){{
        { self_renderer::instance, (renderer_parameter)"loader" }
    }};

    boost::array<unsigned char, 16> checkerboard_texture = {
        0,   0,   255, 255,
        0,   0,   255, 255,
        255, 255, 0,   0,
        255, 255, 0,   0
    };
    boost::array<unsigned char, 64> arrows_texture = {
        0,   255, 255, 255, 255, 255, 255, 0,
        0,   255, 255, 255, 255, 255, 255, 0,
        0,   0,   255, 255, 255, 255, 0,   0,
        0,   0,   255, 255, 255, 255, 0,   0,
        0,   0,   255, 255, 255, 255, 0,   0,
        0,   0,   0,   255, 255, 0,   0,   0,  
        0,   0,   0,   255, 255, 0,   0,   0,  
        0,   0,   0,   255, 255, 0,   0,   0
    };

    glGenTextures(1, &goal::_goal_texture);
    glBindTexture(GL_TEXTURE_2D, goal::_goal_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_LUMINANCE,
        4, 4, 0,
        GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*)&checkerboard_texture
    );

    glGenTextures(1, &stage_exit::arrow_texture);
    glBindTexture(GL_TEXTURE_2D, stage_exit::arrow_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_LUMINANCE,
        8, 8, 0,
        GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*)&arrows_texture
    );
}

void
global_finish_tripwires()
{
    glDeleteTextures(1, &goal::_goal_texture);
}

void
portal::on_trip(thing &o)
{
    board::change_board_with<goal_transition>(next_board(board::current()->name));
}

bool
portal::can_trip(thing &o)
{
    return o.flags & PLAYER;
}

void
portal::draw_self() const
{
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindTexture(GL_TEXTURE_2D, _texture());

    if (universe::instance.achieved_goals[board::current()->name.goal_number()])
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    else
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&_vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&_texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void
goal::on_trip(thing &o)
{
    universe::instance.achieved_goals[board::current()->name.goal_number()] = 1;
    portal::on_trip(o);
}

// arm
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

void
alarm::on_trip(thing &o)
{
    if (label)
        board::current()->fire_trigger(label, &o);
    if (!multiple)
        board::current()->remove_thing(this);
}

bool
alarm::can_trip(thing &o)
{
    return o.flags & PLAYER;
}

void
loader::draw_self() const
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(matrix);

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, board::current()->hud_font->texture);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    BOOST_FOREACH (std::string const &line, descriptor) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        font::draw_string(line);
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glTranslatef(0.0f, -font::GLYPH_VERTEX_SIZE.y, 0.0f);
    }

    glPopMatrix();
}

void
loader::on_trip(thing &o)
{
    universe::set_name(universe_name);
    universe::instance.load(universe_name);
    board::change_board_with<goal_transition>(universe::instance.current_map);
}

void
loader::trigger(thing *scapegoat)
{
    descriptor.clear();
    descriptor.push_back("NEW GAME");
    _set_matrix();
}

bool
loader::can_trip(thing &o)
{
    return o.flags & PLAYER;
}

void
loader::_set_descriptor()
{
    universe temp;
    temp.load(universe_name);

    if (!temp.saved)
        descriptor.push_back("NEW GAME");
    else {
        descriptor.push_back(
            std::string(temp.current_map)
                + " (" + boost::lexical_cast<std::string>(temp.achieved_goals.count())
                + " complete)"
        );

        struct stat s;
        int err = stat(universe::filename(universe_name).c_str(), &s);
        if (err == 0) {
            time_t mtime_secs = s.st_mtimespec.tv_sec;
            struct tm mtime;
            localtime_r(&mtime_secs, &mtime);

            char mtime_string[19];
            strftime(mtime_string, 19, "%Y-%m-%d %H:%M", &mtime);
            descriptor.push_back(mtime_string);
        }
    }
}

// arm
void
loader::_set_matrix()
{
    vec2 scaled_y = normal * TEXT_SCALE;
    if (scaled_y.y < 0.0f)
        scaled_y = -scaled_y;

    vec2 scaled_x = -vperp(scaled_y);

    int size = 0;
    BOOST_FOREACH (std::string const &line, descriptor) {
        if (line.size() > size)
            size = line.size();
    }

    vec2 offset_center = center
        - scaled_x * font::GLYPH_VERTEX_SIZE.x * 0.5f * size
        + scaled_y * font::GLYPH_VERTEX_SIZE.y * 0.5f * (descriptor.size()-1);

    memset(matrix, 0, sizeof(matrix));
    matrix[ 0] = scaled_x.x;
    matrix[ 1] = scaled_x.y;
    matrix[ 4] = scaled_y.x;
    matrix[ 5] = scaled_y.y;
    matrix[10] = 1.0f;
    matrix[12] = offset_center.x;
    matrix[13] = offset_center.y;
    matrix[15] = 1.0f;
}

}
