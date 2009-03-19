#include "board.hpp"
#include "finally.hpp"
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
    _grid.sort_statics();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
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
    if (t->label)
        _things_by_label[t->label].insert(t);
    _grid.add_thing(t);
}

void
board::remove_thing(thing *t)
{
    if (_all_things.find(t) == _all_things.end())
        return;

    if (t->label)
        _things_by_label[t->label].insert(t);
    _dying_things.insert(t);
}

namespace {
    bool _is_not_overlapping_time_vfp2()
    {
        bool r;

        __asm__ (
            "fmrs    r1, s0\n\t"

            "bic     r1, r1, #0x80000000\n\t"
            "teq     r1, %[infinity]\n\t"
            "moveq   %[r], #0\n\t"
            "movne   %[r], #1\n\t"

            "cmp     r1, %[threshold]\n\t"
            "movle   %[r], #0\n\t"

            : [r] "+r" (r)
            : [infinity] "r" (INFINITYF), [threshold] "r" (0.01f)
            : "cc", "r1"
        );
        return r;
    }

    bool _is_collision_time_vfp2(float collide_time)
    {
        bool r;

        __asm__ (
            "fmrs    r1, s0\n\t"

            "tst     r1, #0x80000000\n\t"
            "moveq   %[r], #1\n\t"
            "movne   %[r], #0\n\t"

            "cmp     r1, %[collide_time]\n\t"
            "movge   %[r], #0\n\t"

            "1:\n\t"
            : [r] "+r" (r)
            : [collide_time] "r" (collide_time)
            : "cc", "r1"
        );
        return r;
    }
}

void
board::_find_collision_in_pair_vfp2(
    grid::cell::iterator ia, grid::cell::iterator ib, collision &c
)
{
    (*ia)->collision_time_vfp2_r(**ib);

    thing *a = *ia, *b = *ib;

    if (_overlapping(a, b)) {
        if (_is_not_overlapping_time_vfp2())
            _remove_overlap(a, b);
    } else if (_is_collision_time_vfp2(c.collide_time)) {
        c = (collision){a, b, vfp_s0()};
    }
}

void
board::_find_collision_in_4_cells_vfp2(
    grid::cell_iterator cell_a,
    grid::cell_iterator cell_b,
    grid::cell_iterator cell_c,
    grid::cell_iterator cell_d,
    collision &c
)
{
    grid::cell::iterator ia, ib;
    for (ia = cell_a->things.begin(); ia != cell_a->dynamic_begin(); ++ia) {
        for (ib = cell_a->dynamic_begin(); ib != cell_a->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_b->dynamic_begin(); ib != cell_b->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_c->dynamic_begin(); ib != cell_c->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_d->dynamic_begin(); ib != cell_d->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }

    for (; ia != cell_a->things.end(); ++ia) {
        for (ib = ia + 1; ib != cell_a->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_b->things.begin(); ib != cell_b->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_c->things.begin(); ib != cell_c->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_d->things.begin(); ib != cell_d->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }

    for (ia = cell_b->things.begin(); ia != cell_b->dynamic_begin(); ++ia) {
        for (ib = cell_c->dynamic_begin(); ib != cell_c->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }

    for (; ia != cell_b->things.end(); ++ia) {
        for (ib = cell_c->things.begin(); ib != cell_c->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }
}

void
board::_find_collision_in_2_cells_vfp2(
    grid::cell_iterator cell_a,
    grid::cell_iterator cell_b,
    collision &c
)
{
    grid::cell::iterator ia, ib;
    for (ia = cell_a->things.begin(); ia != cell_a->dynamic_begin(); ++ia) {
        for (ib = cell_a->dynamic_begin(); ib != cell_a->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_b->dynamic_begin(); ib != cell_b->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }

    for (; ia != cell_a->things.end(); ++ia) {
        for (ib = ia + 1; ib != cell_a->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
        for (ib = cell_b->things.begin(); ib != cell_b->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }
}

void
board::_find_collision_in_cell_vfp2(
    grid::cell_iterator cell_a,
    collision &c
)
{
    grid::cell::iterator ia, ib;
    for (ia = cell_a->things.begin(); ia != cell_a->dynamic_begin(); ++ia) {
        for (ib = cell_a->dynamic_begin(); ib != cell_a->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }

    for (; ia != cell_a->things.end(); ++ia) {
        for (ib = ia + 1; ib != cell_a->things.end(); ++ib)
            _find_collision_in_pair_vfp2(ia, ib, c);
    }
}

board::collision
board::_find_collision(grid::cell_area live_area)
{
    collision c = { NULL, NULL, INFINITYF };

    vfp_length_2();

    grid::cell_iterator row, cell;
    int pitch = _grid.pitch();
    for (row = live_area.start; row < live_area.end; row += pitch) {
        for (cell = row; cell - row < live_area.width; ++cell)
            // Middle cell
            _find_collision_in_4_cells_vfp2(cell, cell+1, cell+pitch, cell+pitch+1, c);
        // Right edge cell
        _find_collision_in_2_cells_vfp2(cell, cell+pitch, c);
    }
    for (cell = live_area.end; cell - live_area.end < live_area.width; ++cell) {
        // Bottom edge cell
        _find_collision_in_2_cells_vfp2(cell, cell+1, c);
    }
    // Bottom-right corner cell
    if (cell->has_dynamic())
        _find_collision_in_cell_vfp2(cell, c);

    vfp_length_1();

    return c;
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

struct _sort_things_in_cell {
    renders_with_map &render_map;

    _sort_things_in_cell(renders_with_map &r) : render_map(r) {}

    void operator()(grid::cell const &c) const
    {
        BOOST_FOREACH (thing *th, c.things) {
            renders_with_range rwr = th->renders_with();
            for (renders_with_pair *rw = rwr.begin(); rw != rwr.end(); ++rw)
                render_map[*rw].push_back(th);
        }
    }
};

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

    _clear_render_map();
    _grid.for_cells_in_rect(camera_rect, _sort_things_in_cell(_render_map));

    BOOST_FOREACH (renders_with_map::value_type const &rwp, _render_map) {
        rwp.first.instance->draw(rwp.second, rwp.first.param);
    }
}

void
board::_draw_background()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

struct _move_things {
    float timeslice;

    _move_things(float t) : timeslice(t) { }

    void operator()(grid::cell &c) const
    {
        for (grid::cell::iterator i = c.dynamic_begin(); i != c.things.end(); ++i) {
            thing *th = *i;
            if (vnorm2(th->velocity) >= MOVEMENT_THRESHOLD)
                th->center += th->velocity*timeslice;
            else
                th->velocity = ZERO_VEC2;
        }
    }
};

struct _tick_things {
    board *b;

    _tick_things(board *bb) : b(bb) {}

    void operator()(grid::cell &c) const
    {
        grid::cell::iterator end = c.things.end();
        for (grid::cell::iterator i = c.dynamic_begin(); i != end; ) {
            thing *th = *i;
            th->velocity *= FRICTION;
            th->tick();
            if (b->_grid.move_thing(th))
                --end;
            else
                ++i;
        }
    }
};

board *
board::from_bin(std::string const &name, FILE *bin)
{
    rect bounds = data_from_bin<rect>(bin);

    board *b = new board(name, bounds);
    try {
        while (thing *t = thing_from_bin(bin)) {
            b->add_thing(t);
            if (t->flags & thing::PLAYER)
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
    boost::optional<std::string> path = resource_filename(name, "bb");
    try {
        FILE *bin = fopen(path->c_str(), "rb");
        if (!bin)
            throw invalid_board("Could not open board " + name);

        finally<FILE*> close_bin(bin, fclose);

        return from_bin(name, bin);

    } catch (std::exception const &x) {
        std::cerr << "Reading board " << name << " failed: " << x.what() << "\n";
        return NULL;
    } catch (...) {
        std::cerr << "Reading board " << name << " failed: ...\n";
        return NULL;
    }
}

// s7 isn't clobbered by find_collision, so force tick_time to be kept there
register float tick_time __asm__ ("s7");

void
board::tick()
{
    _kill_dying_things();

    tick_time = 1.0f;
    int rounds = 0;

    grid::cell_area live_area = _grid.cell_area_for_rect(
        make_rect(camera.center - make_vec2(LIVE_RADIUS), camera.center + make_vec2(LIVE_RADIUS))
    );

    while (tick_time > 0.0f && rounds < 100) {
        collision c = _find_collision(live_area);
        _grid.for_cells_in_cell_area(
            live_area,
            _move_things(tick_time < c.collide_time ? tick_time : c.collide_time)
        );
        if (c.collide_time <= tick_time)
            _collide_things(c.a, c.b);

        tick_time -= c.collide_time;
        ++rounds;
    }
#ifdef BENCHMARK
    std::cout << "-- rounds " << rounds << "\n";
#endif
    _grid.for_cells_in_cell_area(live_area, _tick_things(this));
    camera.tick();
    particles.tick();

    ++_tick_count;
}

}
