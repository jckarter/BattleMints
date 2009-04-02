#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "grid.hpp"
#include "controller.hpp"
#include "board_loader.hpp"
#include "particles.hpp"

namespace battlemints {

struct thing;
struct player;
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
    static const unsigned BOARD_MAGIC = 0xBA7713BD;
    static const int BOARD_VERSION = 4;

    static const vec2 CELL_SIZE;
    static const float LIVE_RADIUS;

    static boost::array<vec2, 4> pause_button_vertices;

    enum flag_values {
        SAFE = 1
    };

    std::string name, theme;
    int flags;
    boost::array<vec4, 2> background_gradient;

    boost::array<vec2, 4> background_vertices;
    boost::array<vec4, 4> background_colors;

    font *hud_font;
    image_texture *pause_button;
    player *player_thing;

    board(std::string const &nm, rect bound, std::string const &theme, boost::array<vec4, 2> const &bg, int f);
    virtual ~board();

    void add_thing(thing *t); // board takes ownership of added things and deletes them when done
    void remove_thing(thing *t);
    bool thing_lives(thing *t) { return _all_things.find(t) != _all_things.end(); }

    virtual void setup();
    virtual void tick();
    virtual void draw();

    static board *from_bin(std::string const &name, FILE *bin);

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
        std::cout << "=== ===\n";
        BOOST_FOREACH (thing *th, _all_things)
            std::cout << *th << "\n";
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
    void _draw_background();
    void _draw_hud();
    void _setup_background_vertices();

    friend struct _tick_things;

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
    collision _find_collision(grid::cell_area live_area);

    void _kill_dying_things();

    template<typename UnaryFunctor>
    void _update_thing(thing *t, UnaryFunctor const &f);

    template<typename BinaryFunctor>
    void _update_2_things(thing *t, thing *u, BinaryFunctor const &f);

    thing_set _all_things;
    thing_set _dying_things;

    boost::unordered_map<symbol, thing_set> _things_by_label;
    renders_with_map _render_map;

    grid _grid;

    void _clear_render_map()
    {
        for (renders_with_map::iterator i = _render_map.begin(); i != _render_map.end(); ++i)
            i->second.clear();
    }

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
