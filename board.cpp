#include "board.hpp"
#include "game.hpp"
#include <functional>
#include <iostream>
#include <limits>

namespace battlemints {

static const rect BOARD_RECT = make_rect(-128.0, -128.0, 128.0, 128.0);
static const vec2 BOARD_CELL_SIZE = make_vec2(4.0, 4.0);

board::board()
    : _visibility_grid(BOARD_RECT, BOARD_CELL_SIZE),
      _collision_grid(BOARD_RECT, BOARD_CELL_SIZE),
      _tick_count(0)
{ }

board::~board()
{
    BOOST_FOREACH (thing *th, _all_things)
        delete th;
}

void
board::add_thing(thing *t)
{
    _all_things.insert(t);

    if (t->does_ticks())
        _tickable_things.insert(t);
    _visibility_grid.add_thing(t, t->visibility_box());
    if (t->does_collisions())
        _collision_grid.add_thing(t, t->collision_box());
}

void
board::remove_thing(thing *t)
{
    if (_all_things.find(t) == _all_things.end())
        return;

    _all_things.erase(t);
    if (t->does_ticks())
        _tickable_things.erase(t);
    _visibility_grid.remove_thing(t, t->visibility_box());
    if (t->does_collisions())
        _collision_grid.remove_thing(t, t->collision_box());

    delete t;
}

void
board::set_camera(thing *t)
{
    if (_all_things.find(t) == _all_things.end())
        return;

    _camera = t;
}

thing *
board::camera()
{
    return _camera;
}

template<typename UnaryFunctor>
inline void
board::_update_thing(thing *t, UnaryFunctor const &f)
{
    rect old_visibility = t->visibility_box();
    rect old_collision  = t->collision_box();

    f(t);

    rect new_visibility = t->visibility_box();
    rect new_collision  = t->collision_box();

    _visibility_grid.move_thing(t, old_visibility, new_visibility);
    _collision_grid.move_thing(t, old_collision, new_collision);
}

template<typename BinaryFunctor>
inline void
board::_update_2_things(thing *t, thing *u, BinaryFunctor const &f)
{
    rect t_old_visibility = t->visibility_box();
    rect t_old_collision  = t->collision_box();
    rect u_old_visibility = u->visibility_box();
    rect u_old_collision  = u->collision_box();

    f(t, *u);

    rect t_new_visibility = t->visibility_box();
    rect t_new_collision  = t->collision_box();
    rect u_new_visibility = u->visibility_box();
    rect u_new_collision  = u->collision_box();

    _visibility_grid.move_thing(t, t_old_visibility, t_new_visibility);
    _collision_grid.move_thing(t, t_old_collision, t_new_collision);

    _visibility_grid.move_thing(u, u_old_visibility, u_new_visibility);
    _collision_grid.move_thing(u, u_old_collision, u_new_collision);
}

inline void
board::_find_collision(thing *&a, thing *& b, float &collide_time)
{
    for (std::vector< std::set<thing*> >::iterator cell = _collision_grid.cells.begin();
         cell != _collision_grid.cells.end();
         ++cell) {
        std::set<thing*>::iterator ia, ib;
        for (ia = cell->begin(); ia != cell->end(); ++ia)
            for (ib = ia, ++ib; ib != cell->end(); ++ib) {
                float pair_time = (*ia)->collision_time(**ib);
                if (pair_time > 0.0 && pair_time < collide_time) {
                    collide_time = pair_time; a = *ia; b = *ib;
                }
            }
    }
}

struct _tick_thing {
    _tick_thing() {}

    inline void
    operator()(thing *t) const
    {
        t->tick();
    }
};

void
board::tick()
{
    float tick_time = 1.0;
    int rounds = 0;

    while (tick_time > 0.0) {
        thing *a, *b;
        float collide_time = std::numeric_limits<float>::infinity();

        _find_collision(a, b, collide_time);
        _move_things(std::min(tick_time, collide_time));
        if (collide_time <= tick_time)
            _update_2_things(a, b, std::mem_fun(&thing::collide));

        tick_time -= collide_time;
        ++rounds;
    }

    BOOST_FOREACH (thing *th, _tickable_things)
        _update_thing(th, _tick_thing());

    ++_tick_count;
}

void
board::draw()
{
    vec2 camera_center = _camera ? _camera->center : make_vec2(0.0);
    rect camera_rect = make_rect(
        camera_center - GAME_WINDOW_UNIT_SIZE/2,
        camera_center + GAME_WINDOW_UNIT_SIZE/2
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-camera_center.x, -camera_center.y, 0.0f);

    BOOST_FOREACH (thing *th, _visibility_grid.things_in_rect(camera_rect))
        th->draw();
}

struct _move_thing {
    float timeslice;
    _move_thing(float t) : timeslice(t) {}

    inline void
    operator()(thing *t) const
    {
        t->center += t->velocity*timeslice;
        t->velocity *= FRICTION;
    }
};

void
board::_move_things(float timeslice)
{
    BOOST_FOREACH (thing *th, _all_things) {
        if (vnorm2(th->velocity) >= MOVEMENT_THRESHOLD)
            _update_thing(th, _move_thing(timeslice));
    }
}

}
