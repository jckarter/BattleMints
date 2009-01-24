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

void
cambot::tick()
{
    center += velocity;
    velocity *= FRICTION;
    if (target && board::current()->thing_lives(target)) {
        vec2 goal = target->center + target->velocity * LEAD_FACTOR;
        vec2 goal_velocity = (goal - center) * FOLLOW_FACTOR;
        if (goal_velocity != ZERO_VEC2)
            velocity += ACCEL * vnormalize(goal_velocity - velocity);
    } else
        target = NULL;
}

board *board::_current = NULL;

board::board(std::string const &nm, rect bound)
    : name(nm),
      camera(),
      particles(),
      _grid(bound, CELL_SIZE),
      _tick_count(0),
      _overlaps()
{
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
    _grid.add_thing(t);
}

void
board::remove_thing(thing *t)
{
    if (_all_things.find(t) == _all_things.end())
        return;
    _dying_things.insert(t);
}

static inline bool _is_overlapping_time(float f)
{
    f = fabsf(f);
    return f <= 0.01f
        || f == INFINITYF;
}

void
board::_find_collision_in_pair(
    grid::cell::iterator ia, grid::cell::iterator ib,
    thing *& a, thing *& b, float &collide_time
)
{
    float pair_time = (*ia)->collision_time(**ib);
    if (_overlapping(*ia, *ib)) {
        if (!_is_overlapping_time(pair_time))
            _remove_overlap(*ia, *ib, pair_time);
    } else if (pair_time >= 0.0f && pair_time < collide_time) {
        collide_time = pair_time; a = *ia; b = *ib;
    }
}

void
board::_find_collision_in_same_cell(
    std::vector<grid::cell>::iterator cell,
    thing *& a, thing *& b, float &collide_time
)
{
    grid::cell::iterator ia, ib;
    for (ia = cell->begin(); ia != cell->end(); ++ia) {
        if (!(*ia)->does_collisions()) continue;

        for (ib = ia, ++ib; ib != cell->end(); ++ib) {
            if (!(*ib)->does_collisions()) continue;
            _find_collision_in_pair(ia, ib, a, b, collide_time);
        }
    }
}

void
board::_find_collision_in_adjoining_cells(
    std::vector<grid::cell>::iterator cell_a,
    std::vector<grid::cell>::iterator cell_b,
    thing *& a, thing *& b, float &collide_time
)
{
    grid::cell::iterator ia, ib;
    for (ia = cell_a->begin(); ia != cell_a->end(); ++ia) {
        if (!(*ia)->does_collisions()) continue;

        for (ib = cell_b->begin(); ib != cell_b->end(); ++ib) {
            if (!(*ib)->does_collisions()) continue;
            _find_collision_in_pair(ia, ib, a, b, collide_time);
        }
    }
}

void
board::_find_collision(thing *& a, thing *& b, float &collide_time)
{
    std::vector<grid::cell>::iterator row, cell, last_row = _grid.cells.end() - _grid.pitch();
    for (row = cell = _grid.cells.begin(); cell != last_row; row += _grid.pitch(), ++cell) {
        for (; cell - row < _grid.pitch() - 1; ++cell) {
            // Middle cell
            if (cell->empty()) continue;
            _find_collision_in_same_cell(cell, a, b, collide_time);
            _find_collision_in_adjoining_cells(cell, cell + 1, a, b, collide_time);
            _find_collision_in_adjoining_cells(cell, cell + _grid.pitch(), a, b, collide_time);
            _find_collision_in_adjoining_cells(cell + 1, cell + _grid.pitch(), a, b, collide_time);
            _find_collision_in_adjoining_cells(cell, cell + _grid.pitch() + 1, a, b, collide_time);
        }
        // Right edge cell
        if (cell->empty()) continue;
        _find_collision_in_same_cell(cell, a, b, collide_time);
        _find_collision_in_adjoining_cells(cell, cell + _grid.pitch(), a, b, collide_time);
    }
    for (; cell < _grid.cells.end() - 1; ++cell) {
        // Bottom edge cell
        if (cell->empty()) continue;
        _find_collision_in_same_cell(cell, a, b, collide_time);
        _find_collision_in_adjoining_cells(cell, cell + 1, a, b, collide_time);
    }
    // Bottom-right corner cell
    _find_collision_in_same_cell(cell, a, b, collide_time);
}

void
board::_collide_things(thing *a, thing *b)
{
    if (a->can_overlap() || b->can_overlap())
        _add_overlap(a, b);
    else
        a->collide(*b);
    a->on_collision(*b);
    b->on_collision(*a);
};

inline void
board::_kill_dying_things()
{
    BOOST_FOREACH (thing *th, _dying_things) {
        _all_things.erase(th);
        _grid.remove_thing(th);
        delete th;
    }
    _dying_things.clear();
}

inline void
board::_tick_thing(thing *t)
{
    t->velocity *= FRICTION;
    t->tick();
    _grid.move_thing(t);
}

void
board::tick()
{
    _kill_dying_things();

    float tick_time = 1.0f;
    int rounds = 0;

    while (tick_time > 0.0f && rounds < 100) {
        thing *a, *b;
        float collide_time = INFINITYF;

        _find_collision(a, b, collide_time);
        _move_things(std::min(tick_time, collide_time));
        if (collide_time <= tick_time)
            _collide_things(a, b);

        tick_time -= collide_time;
        ++rounds;
    }
#ifdef BENCHMARK
    std::cout << "-- rounds " << rounds << "\n";
#endif
    BOOST_FOREACH (thing *th, _all_things) {
        _tick_thing(th);
    }
    camera.tick();
    particles.tick();

    ++_tick_count;
}

void
board::draw()
{
    _draw_background();

    vec2 cam_center = camera.center;
    rect camera_rect = make_rect(
        cam_center - GAME_WINDOW_UNIT_SIZE/2.0f - CELL_SIZE,
        cam_center + GAME_WINDOW_UNIT_SIZE/2.0f + CELL_SIZE
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-cam_center.x, -cam_center.y, 0.0f);

    particles.draw();
    std::set<thing*> visible_things = _grid.things_in_rect(camera_rect);
#ifdef BENCHMARK
    std::cout << "-- visible " << visible_things.size() << "\n";
#endif
    BOOST_FOREACH (thing *th, visible_things) {
        th->draw();
    }
    //_grid._draw();
}

void
board::_draw_background()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
board::_move_things(float timeslice)
{
    BOOST_FOREACH (thing *th, _all_things) {
        if (vnorm2(th->velocity) >= MOVEMENT_THRESHOLD)
            th->center += th->velocity*timeslice;
        else
            th->velocity = ZERO_VEC2;
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
                b->camera.cut_to_target(t);
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
