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

struct goal : tripwire {
    static boost::array<renders_with_pair, 1> renders_with_pairs;

    std::string next_board;
    int goal_number;
    int achieves_goal;

    goal(vec2 pt_a, vec2 pt_b, std::string const &nb, int gn, bool ag)
        : tripwire(pt_a, pt_b, 0), next_board(nb),
          goal_number(gn), achieves_goal(ag)
        { _set_up_vertices(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual void draw_self() const;
    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    virtual char const * kind() const { return "goal"; }

    static void global_start();
    static void global_finish();

    goal(FILE *bin)
        : tripwire(0, bin), next_board(*pascal_string_from_bin(bin)),
          goal_number(data_from_bin<int>(bin)), achieves_goal(data_from_bin<int>(bin))
        { _set_up_vertices(); }

    virtual void print(std::ostream &os) const
        { tripwire::print(os); os << " next-board:" << next_board; }

    static GLuint _goal_texture, _arrow_texture;

private:
    void _set_up_vertices();
    boost::array<vec2, 4> _vertices;
    boost::array<vec2, 4> _texcoords;
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
