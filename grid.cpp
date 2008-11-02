#include "grid.hpp"
#include <boost/ref.hpp>
#include <cmath>
#include <algorithm>

namespace battlemints {

struct _set_adjoin {
    _set_adjoin() {}

    template<typename FromPointer>
    inline void
    operator()(FromPointer from, std::set<thing *> & to) const
    {
        std::copy(from->begin(), from->end(), std::inserter(to, to.begin()));
    }
};

struct _insert {
    _insert() {}

    template<typename FromPointer>
    inline void
    operator()(FromPointer from, thing *t) const
    {
        from->insert(t);
    }
};

struct _erase {
    _erase() {}

    template<typename FromPointer>
    inline void
    operator()(FromPointer from, thing *t) const
    {
        from->erase(t);
    }
};

grid::grid(rect space, vec2 cell_size)
    : _origin(space.low),
      _cell_size_inv(1/cell_size),
      _pitch((int)ceilf((space.high.x - space.low.x) * _cell_size_inv.x)),
      cells((unsigned)vproduct(vceil((space.high - space.low) * _cell_size_inv)))
{ }

template <typename T, typename BinaryFunctor>
inline void
grid::_for_cells_in_rect(T t, rect bound, BinaryFunctor const &f) const
{
    std::vector< std::set<thing*> >::const_iterator start = cell_for_point(bound.low);
    std::vector< std::set<thing*> >::const_iterator right = cell_for_point(rect_lr(bound));
    unsigned width = right - start + 1;
    std::vector< std::set<thing*> >::const_iterator end = cell_for_point(bound.high);

    for (std::vector< std::set<thing*> >::const_iterator y = start; y <= end; y += _pitch)
        for (std::vector< std::set<thing*> >::const_iterator x = y; x - y <= width; ++x)
            f(x, t);
}

template <typename T, typename BinaryFunctor>
inline void
grid::_for_cells_in_rect(T t, rect bound, BinaryFunctor const &f)
{
    std::vector< std::set<thing*> >::iterator start = cell_for_point(bound.low);
    std::vector< std::set<thing*> >::iterator right = cell_for_point(rect_lr(bound));
    unsigned width = right - start + 1;
    std::vector< std::set<thing*> >::iterator end = cell_for_point(bound.high);

    for (std::vector< std::set<thing*> >::iterator y = start; y <= end; y += _pitch)
        for (std::vector< std::set<thing*> >::iterator x = y; x - y <= width; ++x)
            f(x, t);
}

void
grid::add_thing(thing *t, rect bound)
{
    _for_cells_in_rect(t, bound, _insert());
}

void
grid::remove_thing(thing *t, rect bound)
{
    _for_cells_in_rect(t, bound, _erase());
}

void
grid::move_thing(thing *t, rect old_bound, rect new_bound)
{
    remove_thing(t, old_bound);
    add_thing(t, new_bound);
}

std::set<thing*>
grid::things_in_rect(rect r) const
{
    std::set<thing *> ret;

    _for_cells_in_rect(boost::ref(ret), r, _set_adjoin());

    return ret;
}

std::vector< std::set<thing*> >::iterator
grid::cell_for_point(vec2 pt)
{
    vec2 coords = (pt - _origin) * _cell_size_inv;
    return cells.begin() + _pitch * (unsigned)coords.y + (unsigned)coords.x;
}

std::vector< std::set<thing*> >::const_iterator
grid::cell_for_point(vec2 pt) const
{
    vec2 coords = (pt - _origin) * _cell_size_inv;
    return cells.begin() + _pitch * (unsigned)coords.y + (unsigned)coords.x;
}

}
