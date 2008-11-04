#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <set>
#include "grid.hpp"
#include "controller.hpp"

namespace battlemints {

typedef std::set<thing*> thing_set;

struct board : controller {

    board(rect bound);
    virtual ~board();

    void add_thing(thing *t); // board takes ownership of added things and deletes them when done
    void remove_thing(thing *t);

    void set_camera(thing *t);
    thing *camera();

    virtual void setup();
    virtual void tick();
    virtual void draw();

    static board *make_demo_board();

private:
    void _draw_background();

    void _find_collision(thing *&a, thing *&b, float &collide_time);
    void _move_things(float timeslice);

    template<typename UnaryFunctor>
    void _update_thing(thing *t, UnaryFunctor const &f);

    template<typename BinaryFunctor>
    void _update_2_things(thing *t, thing *u, BinaryFunctor const &f);

    thing_set _all_things;
    thing_set _tickable_things;

    thing *_camera;

    grid _visibility_grid;
    grid _collision_grid;

    unsigned long _tick_count;
};

}

#endif
