#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <iostream>
#include <string>
#include <stdexcept>
#include <json/json.h>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "grid.hpp"
#include "controller.hpp"
#include "board_loader.hpp"
#include "particles.hpp"

namespace battlemints {

struct thing;
struct tile_vertices;

typedef boost::unordered_set<thing*> thing_set;

struct cambot {
    static const float LEAD_FACTOR, FOLLOW_FACTOR, ACCEL;

    thing *target;
    vec2 center, velocity;

    cambot() : target(NULL), center(ZERO_VEC2), velocity(ZERO_VEC2) { }
    void tick();
    void cut_to_target(thing *new_target) { target = new_target; center = new_target->center; }
};

struct board : controller {
    static const vec2 CELL_SIZE;

    std::string name;

    board(std::string const &nm, rect bound);
    virtual ~board();

    void add_thing(thing *t); // board takes ownership of added things and deletes them when done
    void remove_thing(thing *t);
    bool thing_lives(thing *t) { return _all_things.find(t) != _all_things.end(); }

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

    void dump_things() const
    {
        std::cerr << "=== ===\n";
        BOOST_FOREACH (thing *th, _all_things)
            std::cerr << *th << "\n";
    }

    unsigned long tick_count() const { return _tick_count; }

    cambot camera;
    particle_system particles;
    tile_vertices *tile_vertices_thing;

    void fire_trigger(symbol s, thing *scapegoat)
    {
        BOOST_FOREACH (thing *th, _things_by_label[s])
            th->trigger(scapegoat);
    }

private:
    void _collide_things(thing *a, thing *b);
    void _tick_thing(thing *th);
    void _draw_background();

    struct collision {
        thing *a, *b;
        float collide_time;
    };

    void _find_collision_in_pair_vfp2(
        grid::cell::iterator ia, grid::cell::iterator ib,
        collision &c
    );
    void _find_collision_in_4_cells_vfp2(
        grid::cell_iterator cell_a,
        grid::cell_iterator cell_b,
        grid::cell_iterator cell_c,
        grid::cell_iterator cell_d,
        collision &c
    );
    void _find_collision_in_2_cells_vfp2(
        grid::cell_iterator cell_a,
        grid::cell_iterator cell_b,
        collision &c
    );
    void _find_collision_in_cell_vfp2(
        grid::cell_iterator cell,
        collision &c
    );
    collision _find_collision();

    void _move_things(float timeslice);
    void _kill_dying_things();

    template<typename UnaryFunctor>
    void _update_thing(thing *t, UnaryFunctor const &f);

    template<typename BinaryFunctor>
    void _update_2_things(thing *t, thing *u, BinaryFunctor const &f);

    thing_set _all_things;
    thing_set _ticking_things;
    thing_set _dying_things;

    boost::unordered_map<symbol, thing_set> _things_by_label;

    grid _grid;

    typedef std::pair<thing *, thing *> overlap_pair;
    std::vector<overlap_pair> _overlaps;
    bool _overlapping(thing *a, thing *b)
        { return !_overlaps.empty()
            && std::find(_overlaps.begin(), _overlaps.end(), std::make_pair(a,b))
               != _overlaps.end(); }
    void _add_overlap(thing *a, thing *b)
        { _overlaps.push_back(std::make_pair(a,b)); }
    void _remove_overlap(thing *a, thing *b)
        { _overlaps.erase(std::find(_overlaps.begin(), _overlaps.end(), std::make_pair(a,b))); }

    unsigned long _tick_count;

    static board *_current;
};

}

#endif
