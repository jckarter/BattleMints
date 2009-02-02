#include "grid.hpp"
#include <boost/ref.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace battlemints {

namespace {
    struct _sort_static_first {
        _sort_static_first() {}

        bool operator()(thing const *a, thing const *b) const
        {
            return !a->does_ticks() && b->does_ticks();
        }
    };
}

void
grid::cell::sort_statics()
{
    std::sort(things.begin(), things.end(), _sort_static_first());
    
    for (dynamic_begin_offset = 0;
         dynamic_begin_offset < things.size() && !(things[dynamic_begin_offset]->does_ticks());
         ++dynamic_begin_offset)
        ;
}

grid::grid(rect space, vec2 cell_size)
    : _origin(space.low),
      _cell_size_inv(1.0f/cell_size),
      _cell_dims(vceil( (space.high-space.low)*_cell_size_inv ) - make_vec2(1.0,1.0)),
      _pitch((int)ceilf((space.high.x - space.low.x) * _cell_size_inv.x)),
      cells((unsigned)vproduct(vceil((space.high - space.low) * _cell_size_inv)))
{ }

void
grid::_draw() const
{
    vec2 cell_size = 1.0f/_cell_size_inv;
    glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    boost::array<vec2, 3> cell_vertices = {
        make_vec2(cell_size.x, 0.0f),
        make_vec2(0.0f, 0.0f),
        make_vec2(0.0f, cell_size.y),
    };

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&cell_vertices[0]);

    float x = _origin.x, y = _origin.y;
    for (std::vector<cell>::const_iterator i = cells.begin(); i != cells.end(); ++i) {
        if (i != cells.begin() && (i - cells.begin()) % _pitch == 0) {
            x = _origin.x;
            y += cell_size.y;
        }
        else
            x += cell_size.x;

        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glDrawArrays(GL_LINE_STRIP, 0, 3);
        glPopMatrix();
    }
}

void
grid::add_thing(thing *t)
{
    cell_for_point(t->center)->things.push_back(t);
}

void
grid::remove_thing(thing *t)
{
    std::vector<grid::cell>::iterator c = cell_for_point(t->prev_center);
    c->things.erase(std::find(c->dynamic_begin(), c->things.end(), t));
}

void
grid::move_thing(thing *t)
{
    std::vector<grid::cell>::iterator old_c = cell_for_point(t->prev_center),
                                      new_c = cell_for_point(t->center);
    t->prev_center = t->center;
    if (old_c != new_c) {
        old_c->things.erase(std::find(old_c->dynamic_begin(), old_c->things.end(), t));
        new_c->things.push_back(t);
    }
}

std::vector<grid::cell>::iterator
grid::cell_for_point(vec2 pt)
{
    vec2 coords = (pt - _origin) * _cell_size_inv;
    coords = vmax(ZERO_VEC2, coords);
    coords = vmin(_cell_dims, coords);

    return cells.begin() + _pitch * (unsigned)coords.y + (unsigned)coords.x;
}

std::vector<grid::cell>::const_iterator
grid::cell_for_point(vec2 pt) const
{
    vec2 coords = (pt - _origin) * _cell_size_inv;
    coords = vmax(ZERO_VEC2, coords);
    coords = vmin(_cell_dims, coords);

    return cells.begin() + _pitch * (unsigned)coords.y + (unsigned)coords.x;
}

}
