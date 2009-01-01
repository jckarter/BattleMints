#include "board.hpp"
#include "game.hpp"
#include "thing.hpp"
#include "dramatis_personae.hpp"
#include "particles.hpp"
#include "walls.hpp"
#include "serialization.hpp"
#include <functional>
#include <iostream>

namespace battlemints {

board *board::_current = NULL;

board::board(std::string const &nm, rect bound)
    : name(nm),
      _visibility_grid(bound, VISIBILITY_CELL_SIZE),
      _collision_grid(bound, COLLISION_CELL_SIZE),
      _tick_count(0),
      _camera_thing(new camera()),
      _particles_thing(new particles(bound)),
      _overlaps()
{
    add_thing(_particles_thing);
    add_thing(_camera_thing);
}

void
board::setup()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glDisable(GL_DEPTH_TEST);

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

    BOOST_FOREACH (thing *th, _all_things)
        th->awaken();
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

static inline bool _is_overlapping_time(float f)
{
    f = fabsf(f);
    return f <= 0.01f
        || f == INFINITYF;
}

inline void
board::_find_collision(thing *& a, thing *& b, float &collide_time)
{
    for (std::vector< std::set<thing*> >::iterator cell = _collision_grid.cells.begin();
         cell != _collision_grid.cells.end();
         ++cell) {
        std::set<thing*>::iterator ia, ib;
        for (ia = cell->begin(); ia != cell->end(); ++ia)
            for (ib = ia, ++ib; ib != cell->end(); ++ib) {
                float pair_time = (*ia)->collision_time(**ib);
                if (_overlapping(*ia, *ib)) {
                    if (!_is_overlapping_time(pair_time))
                        _remove_overlap(*ia, *ib, pair_time);
                } else if (pair_time >= 0.0 && pair_time < collide_time) {
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
        t->velocity *= FRICTION;
        t->tick();
    }
};

struct _collide_things {
    board &bo;
    _collide_things(board &bb) : bo(bb) {}

    inline void
    operator()(thing *a, thing *b) const
    {
        //std::cerr << "collide " << bo.tick_count()
        //          <<  "\n    [" << *a << "]\n    [" << *b << "]\n";
        if (a->can_overlap() || b->can_overlap())
            bo._add_overlap(a, b);
        else
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

    while (tick_time > 0.0 && rounds < 100) {
        thing *a, *b;
        float collide_time = INFINITYF;

        _find_collision(a, b, collide_time);
        _move_things(std::min(tick_time, collide_time));
        if (collide_time <= tick_time)
            _update_2_things(a, b, _collide_things(*this));

        tick_time -= collide_time;
        ++rounds;
    }

    if (rounds >= 100)
        std::cerr << "More than 100 collision rounds!\n";

    BOOST_FOREACH (thing *th, _all_things)
        _update_thing(th, _tick_thing());

    ++_tick_count;
}

void
board::draw()
{
    _draw_background();

    vec2 cam_center = _camera_thing->center;
    rect camera_rect = make_rect(
        cam_center - GAME_WINDOW_UNIT_SIZE/2,
        cam_center + GAME_WINDOW_UNIT_SIZE/2
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-cam_center.x, -cam_center.y, 0.0f);

    std::set<thing*> visible_things = _visibility_grid.things_in_rect(camera_rect);
    BOOST_FOREACH (thing *th, visible_things) {
        th->draw();
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
    }
};

struct _stop_thing {
    _stop_thing() {}

    inline void
    operator()(thing *t) const
    {
        t->velocity = ZERO_VEC2;
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
board::from_json(std::string const &name, Json::Value const &v)
{
    if (!v.isObject())
        throw invalid_board_json("Root of board JSON must be Object");

    rect bounds = rect_from_json(v["bounds"]);
    Json::Value things = v["things"];
    if (!things.isArray())
        throw invalid_board_json("\"things\" field of board JSON must be Array");

    board *b = new board(name, bounds);
    try {
        for (unsigned i = 0; i < things.size(); ++i) {
            Json::Value const &tv = things[i];
            thing *t = thing_from_json(tv);
            b->add_thing(t);
            if (tv[1].isMember("camera"))
                b->camera_thing()->cut_to_target(t);
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
        return from_json(name, json);
    } catch (std::exception const &x) {
        std::cerr << "Reading board " << name << " failed: " << x.what() << "\n";
        return NULL;
    } catch (...) {
        std::cerr << "Reading board " << name << " failed: ...\n";
        return NULL;
    }
}

}
