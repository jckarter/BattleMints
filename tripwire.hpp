#ifndef __TRIPWIRE_HPP__
#define __TRIPWIRE_HPP__

#include "thing.hpp"
#include "board.hpp"
#include <OpenGLES/ES1/gl.h>

namespace battlemints {

void global_start_tripwires();
void global_finish_tripwires();

struct tripwire : line {
    tripwire(vec2 pt_a, vec2 pt_b, int flags)
        : line(pt_a, pt_b, CAN_OVERLAP | LAYER_2 | DOES_TICKS | flags) { }

    /* no physical collision */
    virtual void collide(thing &o) { }
    virtual void collide_sphere(sphere &o) { }
    virtual void collide_line(line &o) { }

    virtual void on_collision(thing &o) {
        if (can_trip(o)) {
            on_trip(o);
        }
    }

    virtual bool can_trip(thing &o) = 0;
    virtual void on_trip(thing &o) = 0;

    virtual char const * kind() const { return "tripwire"; }

protected:
    tripwire(int flags, FILE *bin) : line(CAN_OVERLAP | LAYER_2 | DOES_TICKS | flags, bin) { }
};

struct portal : tripwire {
    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual void draw_self() const;

    virtual board_name next_board(board_name current) const = 0;
    virtual bool achieves_goal() const { return false; }

    portal(FILE *bin) : tripwire(0, bin) { _set_up_vertices(); }

protected:
    virtual GLuint _texture() const = 0;
private:
    void _set_up_vertices();
    boost::array<vec2, 4> _vertices;
    boost::array<vec2, 4> _texcoords;
};

struct goal : portal {
    static boost::array<renders_with_pair, 1> renders_with_pairs;
    static GLuint goal_texture;

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual char const * kind() const { return "goal"; }

    virtual board_name next_board(board_name current) const { return current.with_stage(0); }
    virtual bool achieves_goal() const { return true; }

    goal(FILE *bin) : portal(bin) { }

protected:
    virtual GLuint _texture() const { return goal_texture; }
};

struct stage_exit : portal {
    static boost::array<renders_with_pair, 1> renders_with_pairs;
    static GLuint arrow_texture;

    int stage_number;

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual char const * kind() const { return "stage_exit"; }

    virtual board_name next_board(board_name current) const
        { return current.with_stage(stage_number); }

    stage_exit(FILE *bin) : portal(bin), stage_number(data_from_bin<int>(bin)) { }

protected:
    virtual GLuint _texture() const { return arrow_texture; }
};

struct world_exit : portal {
    static boost::array<renders_with_pair, 1> renders_with_pairs;
    static GLuint arrow_texture;

    int world_number;

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual char const * kind() const { return "world_exit"; }

    virtual board_name next_board(board_name current) const
        { return board_name::make(world_number, 0); }
    virtual bool achieves_goal() const { return true; }

    world_exit(FILE *bin) : portal(bin), world_number(data_from_bin<int>(bin)) { }

protected:
    virtual GLuint _texture() const { return goal::goal_texture; }
};

struct alarm : tripwire {
    bool multiple;

    alarm(vec2 pt_a, vec2 pt_b) : tripwire(pt_a, pt_b, 0), multiple(false) { }

    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual char const * kind() const { return "alarm"; }

    alarm(FILE *bin) : tripwire(0, bin), multiple(false) { }
};

struct loader : tripwire {
    static const float TEXT_SCALE;
    static boost::array<renders_with_pair, 1> renders_with_pairs;

    std::string universe_name;
    std::vector<std::string> descriptor;
    GLfloat matrix[16];

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual void draw_self() const;
    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual char const * kind() const { return "loader"; }

    virtual void trigger(thing *scapegoat);

    loader(FILE *bin)
        : tripwire(0, bin), universe_name(*pascal_string_from_bin(bin))
        { _set_descriptor(); _set_matrix(); }

private:
    std::string _make_descriptor();

    void _set_descriptor();
    void _set_matrix();
};

}

#endif
