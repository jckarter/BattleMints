#include "board.hpp"
#include "game.hpp"
#include "thing.hpp"
#include "sphere.hpp"
#include "player.hpp"
#include "wall.hpp"
#include "serialization.hpp"
#include <functional>
#include <iostream>
#include <limits>

namespace battlemints {

static const vec2 BOARD_COLLISION_CELL_SIZE = make_vec2(2.0, 2.0);
static const vec2 BOARD_VISIBILITY_CELL_SIZE = make_vec2(2.0, 2.0);

board *board::_current = NULL;

board::board(rect bound)
    : _camera(NULL),
      _visibility_grid(bound, BOARD_VISIBILITY_CELL_SIZE),
      _collision_grid(bound, BOARD_COLLISION_CELL_SIZE),
      _tick_count(0)
{}

void
board::setup()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(
        -160.0f * GAME_UNITS_PER_PIXEL, 160.0f * GAME_UNITS_PER_PIXEL,
        -240.0f * GAME_UNITS_PER_PIXEL, 240.0f * GAME_UNITS_PER_PIXEL,
        -16.0f, 16.0f
    );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLineWidth(BORDER_THICKNESS * PIXELS_PER_GAME_UNIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    _current = this;
}

board::~board()
{
    _kill_dying_things();
    BOOST_FOREACH (thing *th, _all_things)
        delete th;
}

void
board::add_thing(thing *t)
{
    _all_things.insert(t);

    if (t->does_ticks())
        _tickable_things.insert(t);
    if (t->does_draws())
        _visibility_grid.add_thing(t, t->visibility_box());
    if (t->does_collisions())
        _collision_grid.add_thing(t, t->collision_box());
}

void
board::remove_thing(thing *t)
{
    if (_all_things.find(t) == _all_things.end())
        return;

    _dying_things.insert(t);
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

    f(t, u);

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
                if (pair_time >= 0.0 && pair_time < collide_time) {
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

struct _collide_things {
    _collide_things() {}

    inline void
    operator()(thing *a, thing *b) const
    {
        a->collide(*b);
        a->on_collision(*b);
        b->on_collision(*a);
    }
};

inline void
board::_kill_dying_things()
{
    BOOST_FOREACH (thing *th, _dying_things) {
        _all_things.erase(th);
        if (th->does_ticks())
            _tickable_things.erase(th);
        if (th->does_draws())
            _visibility_grid.remove_thing(th, th->visibility_box());
        if (th->does_collisions())
            _collision_grid.remove_thing(th, th->collision_box());

        delete th;
    }
    _dying_things.clear();
}

void
board::tick()
{
    _kill_dying_things();

    float tick_time = 1.0;
    int rounds = 0;

    while (tick_time > 0.0) {
        thing *a, *b;
        float collide_time = std::numeric_limits<float>::infinity();

        _find_collision(a, b, collide_time);
        _move_things(std::min(tick_time, collide_time));
        if (collide_time <= tick_time)
            _update_2_things(a, b, _collide_things());

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
    _draw_background();

    vec2 camera_center = _camera ? _camera->center : make_vec2(0.0);
    rect camera_rect = make_rect(
        camera_center - GAME_WINDOW_UNIT_SIZE/2,
        camera_center + GAME_WINDOW_UNIT_SIZE/2
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-camera_center.x, -camera_center.y, 0.0f);

    std::set<thing*> visible_things = _visibility_grid.things_in_rect(camera_rect);
    BOOST_FOREACH (thing *th, visible_things) {
        th->draw();
    }

    if (_tick_count % 300 == 0) {
        std::cerr << visible_things.size() << " visible things\n";
    }
}

void
board::_draw_background()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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

struct _stop_thing {
    _stop_thing() {}

    inline void
    operator()(thing *t) const
    {
        t->velocity = make_vec2(0.0);
    }
};

void
board::_move_things(float timeslice)
{
    BOOST_FOREACH (thing *th, _all_things) {
        if (vnorm2(th->velocity) >= MOVEMENT_THRESHOLD)
            _update_thing(th, _move_thing(timeslice));
        else
            _update_thing(th, _stop_thing());
    }
}

board *
board::from_json(Json::Value const &v)
{
    if (!v.isObject())
        throw invalid_board_json("Root of board JSON must be Object");

    rect bounds = rect_from_json(v["bounds"]);
    Json::Value things = v["things"];
    if (!things.isArray())
        throw invalid_board_json("\"things\" field of board JSON must be Array");

    board *b = new board(bounds);
    try {
        for (unsigned i = 0; i < things.size(); ++i) {
            Json::Value const &tv = things[i];
            thing *t = thing_from_json(tv);
            b->add_thing(t);
            if (tv[1].isMember("camera"))
                b->set_camera(t);
        }
    } catch (...) {
        delete b;
        throw;
    }

    return b;
}

board *
board::from_file(std::string const &name)
{
    boost::optional<std::string> path = resource_filename(name, "board");
    try {
        Json::Value json;
        {
            std::ifstream file(path->c_str());
            file >> json;
        }
        return from_json(json);
    } catch(std::exception const &x) {
        std::cerr << "Reading board " << name << " failed: " << x.what() << "\n";
        return NULL;
    } catch(...) {
        std::cerr << "Reading board " << name << " failed: ...\n";
        return NULL;
    }
}

}
