#ifndef __GOAL_HPP__
#define __GOAL_HPP__

#include "tripwire.hpp"
#include <OpenGLES/ES1/gl.h>

namespace battlemints {

struct goal : tripwire {
    std::string next_board;

    goal(vec2 pt_a, vec2 pt_b, std::string const &nb)
        : tripwire(pt_a, pt_b), next_board(nb) { _set_up_vertices(); }

    virtual void draw();
    virtual void on_trip(thing &o);
    virtual bool can_trip(thing &o);

    static thing *from_json(Json::Value const &v);
    static void global_start();
    static void global_finish();

private:
    void _set_up_vertices();
    boost::array<vec2, 4> _vertices;
    boost::array<vec2, 4> _texcoords;

    static GLuint _goal_texture;
};

}

#endif
