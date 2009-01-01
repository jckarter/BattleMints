#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <iostream>
#include <set>
#include <string>
#include <stdexcept>
#include <json/json.h>
#include <boost/foreach.hpp>
#include "grid.hpp"
#include "controller.hpp"
#include "board_loader.hpp"

namespace battlemints {

struct thing;

typedef std::set<thing*> thing_set;

struct exhaust;

struct board : controller {
    static const vec2 COLLISION_CELL_SIZE, VISIBILITY_CELL_SIZE;

    std::string name;

    board(std::string const &nm, rect bound);
    virtual ~board();

    void add_thing(thing *t); // board takes ownership of added things and deletes them when done
    void remove_thing(thing *t);
    void replace_thing(thing *olde, thing *nu);
    bool thing_lives(thing *t) { return _all_things.find(t) != _all_things.end(); }

    void set_camera(thing *t);
    thing *camera();

    exhaust *exhaust_thing() { return _exhaust_thing; }

    virtual void setup();
    virtual void tick();
    virtual void draw();

    static board *make_demo_board();
    static board *from_json(std::string const &name, Json::Value const &v);
    static board *from_file(std::string const &name);

    static board *current() { return _current; }

    template<typename Transition>
    static void change_board_with(std::string const &name)
    {
        if (current() == controller::current())
            controller::set_current(
                new Transition(current(), new board_loader(name))
            );
    }
    template<typename Transition>
    static void restart_with()
    {
        change_board_with<Transition>(current()->name);
    }

    vec2 camera_velocity() const;
    vec2 camera_center() const;

    void dump_things() const
    {
        std::cerr << "=== ===\n";
        BOOST_FOREACH (thing *th, _all_things)
            std::cerr << *th << "\n";
    }

    unsigned long tick_count() const { return _tick_count; }

private:
    friend struct _collide_things;

    void _draw_background();
    void _find_collision(thing *&a, thing *&b, float &collide_time);
    void _move_things(float timeslice);
    void _kill_dying_things();

    template<typename UnaryFunctor>
    void _update_thing(thing *t, UnaryFunctor const &f);

    template<typename BinaryFunctor>
    void _update_2_things(thing *t, thing *u, BinaryFunctor const &f);

    thing_set _all_things;
    thing_set _dying_things;

    thing *_camera;
    exhaust *_exhaust_thing;

    grid _visibility_grid;
    grid _collision_grid;

    typedef std::pair<thing *, thing *> overlap_pair;
    std::set<overlap_pair> _overlaps;
    bool _overlapping(thing *a, thing *b)
        { return _overlaps.find(std::make_pair(a,b)) != _overlaps.end(); }
    void _add_overlap(thing *a, thing *b)
        { _overlaps.insert(std::make_pair(a,b)); }
    void _remove_overlap(thing *a, thing *b, float f)
        { _overlaps.erase(std::make_pair(a,b)); }

    unsigned long _tick_count;

    static board *_current;
};

}

#endif
