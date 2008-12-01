#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <set>
#include <string>
#include <stdexcept>
#include <json/json.h>
#include "grid.hpp"
#include "controller.hpp"
#include "sound_server.hpp"

namespace battlemints {

typedef std::set<thing*> thing_set;

struct exhaust;

struct board : controller {

    board(rect bound);
    virtual ~board();

    void add_thing(thing *t); // board takes ownership of added things and deletes them when done
    void remove_thing(thing *t);
    void replace_thing(thing *olde, thing *nu);
    bool thing_lives(thing *t) { return _all_things.find(t) != _all_things.end(); }

    void set_camera(thing *t);
    thing *camera();

    exhaust *exhaust_thing() { return _exhaust_thing; }
    sound_server *sound() { return _sound; }

    virtual void setup();
    virtual void tick();
    virtual void draw();

    static board *make_demo_board();
    static board *from_json(Json::Value const &v);
    static board *from_file(std::string const &name);

    static board *current() { return _current; }

    vec2 camera_velocity() const;
    vec2 camera_center() const;

private:
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
    sound_server *_sound;

    grid _visibility_grid;
    grid _collision_grid;

    unsigned long _tick_count;

    static board *_current;
};

}

#endif
